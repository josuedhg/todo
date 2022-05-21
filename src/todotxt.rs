use crate::{Task, Todo};
use std::fs::File;
use std::io::{BufRead, BufReader};
use std::io::{BufWriter, Write, Seek};

pub trait TodotxtIO {
    fn read_tasks(&mut self) -> Vec<Task>;
    fn write_tasks(&mut self, tasks: &mut Vec<Task>);
}

impl TodotxtIO for File {
    fn read_tasks(&mut self) -> Vec<Task> {
        let mut tasks = Vec::new();
        let mut line = String::new();
        BufReader::new(self).lines().for_each(|l| {
            line = l.unwrap();
            if line.starts_with("#") {
                return;
            }
            let task = Task::from_str(&line);
            tasks.push(task);
        });
        tasks
    }

    fn write_tasks(&mut self, tasks: &mut Vec<Task>) {
        let mut writer = BufWriter::new(self);
        writer.rewind().unwrap();
        for task in tasks {
            let line = format!("{}\n", task.to_str());
            writer.write(line.as_bytes()).unwrap();
        }
    }
}

pub struct TodoTxt {
    io: Box<dyn TodotxtIO>,
    tasks: Vec<Task>,
}

impl TodoTxt {
    pub fn new(io: Box<dyn TodotxtIO>) -> TodoTxt {
        TodoTxt {
            io,
            tasks: Vec::new(),
        }
    }
}

impl Todo for TodoTxt {
    fn add(&mut self, task: Task) {
        self.tasks.append(&mut self.io.read_tasks());
        self.tasks.push(task);
        self.io.write_tasks(&mut self.tasks);
    }
    fn remove(&mut self, index: usize) {
        self.tasks.append(&mut self.io.read_tasks());
        self.tasks.remove(index);
        self.io.write_tasks(&mut self.tasks);
    }
    fn list(&mut self) -> Vec<Task> {
        self.tasks.append(&mut self.io.read_tasks());
        self.tasks.clone()
    }
}

#[cfg(test)]
mod test {
    use super::*;

    struct MockIO {
        tasks: Vec<Task>,
    }

    impl MockIO {
        fn new() -> MockIO {
            MockIO { tasks: Vec::new() }
        }
    }

    impl TodotxtIO for MockIO {
        fn read_tasks(&mut self) -> Vec<Task> {
            let mut ret = Vec::new();
            ret.append(&mut self.tasks);
            ret
        }
        fn write_tasks(&mut self, tasks: &mut Vec<Task>) {
            self.tasks.append(tasks);
        }
    }

    #[test]
    fn test_add() {
        let mut todo = TodoTxt::new(Box::new(MockIO::new()));
        todo.add(Task::new("test".to_string(), "project".to_string(), 'A'));
        assert_eq!(todo.list().len(), 1);
    }

    #[test]
    fn test_remove() {
        let mut todo = TodoTxt::new(Box::new(MockIO::new()));
        todo.add(Task::new("test".to_string(), "project".to_string(), 'A'));
        todo.remove(0);
        assert_eq!(todo.list().len(), 0);
    }

    #[test]
    fn test_list() {
        let mut todo = TodoTxt::new(Box::new(MockIO::new()));
        todo.add(Task::new("test".to_string(), "project".to_string(), 'A'));
        assert_eq!(todo.list().len(), 1);
    }

    #[test]
    fn test_list_empty() {
        let mut todo = TodoTxt::new(Box::new(MockIO::new()));
        assert_eq!(todo.list().len(), 0);
    }
}
