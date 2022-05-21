use crate::{Task, Todo};
use std::fs::File;
use std::io::{BufRead, BufReader};
use std::io::{BufWriter, Write, Seek};
use std::str::FromStr;
use std::string::{ToString, ParseError};

pub trait TodotxtIO {
    fn read_tasks(&mut self) -> Vec<Task>;
    fn write_tasks(&mut self, tasks: &mut Vec<Task>);
}

impl FromStr for Task {
    type Err = ParseError;

    fn from_str(s: &str) -> Result<Task, Self::Err> {
        let completed = s.starts_with("x");
        let x: &[_] = &['x', ' ', '(' , ')'];
        let priority = s.trim_matches(x).chars().next().unwrap();
        let name = s.trim_start_matches(x).trim_start_matches(char::is_alphabetic).trim_start_matches(x);

        let mut project = String::new();
        for word in name.split_whitespace() {
            if word.starts_with("+") {
                project = word.to_string().trim_start_matches('+').to_string();
                break;
            }
        }

        let mut task = Task::new(name.to_string(), project.to_string(), priority);
        if completed {
            task.complete();
        }
        Ok(task)
    }
}

impl ToString for Task {
    fn to_string(&self) -> String {
        let mut s = String::new();
        if self.is_complete() {
            s.push('x');
            s.push(' ');
        }
        s.push('(');
        s.push(self.get_priority());
        s.push(')');
        s.push(' ');
        s.push_str(&self.name);
        if !self.project.is_empty() && !self.name.contains(format!("+{}", self.project).as_str()) {
            s.push(' ');
            s.push('+');
            s.push_str(&self.project);
        }
        s
    }

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
            tasks.push(task.unwrap());
        });
        tasks
    }

    fn write_tasks(&mut self, tasks: &mut Vec<Task>) {
        let mut writer = BufWriter::new(self);
        writer.rewind().unwrap();
        for task in tasks {
            let line = format!("{}\n", task.to_string());
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

    #[test]
    fn test_task_from_str_no_project() {
        let task = Task::from_str("(A) Learn Rust").unwrap();
        assert_eq!(task.name, "Learn Rust");
        assert_eq!(task.project, "");
        assert_eq!(task.get_priority(), 'A');
    }

    #[test]
    fn test_task_from_str() {
        let task = Task::from_str("x (A) Learn Rust +project").unwrap();
        assert_eq!(task.name, "Learn Rust +project");
        assert_eq!(task.project, "project");
        assert_eq!(task.is_complete(), true);
        assert_eq!(task.get_priority(), 'A');
    }

    #[test]
    fn test_task_from_str_completed() {
        let task = Task::from_str("x (A) Learn Rust +project").unwrap();
        assert_eq!(task.is_complete(), true);
    }

    #[test]
    fn test_task_to_str_no_project() {
        let task = Task::new(String::from("Learn Rust"), String::from(""), 'A');
        assert_eq!(task.to_string(), "(A) Learn Rust");
    }

    #[test]
    fn test_task_to_str() {
        let task = Task::new(String::from("Learn Rust"), String::from("project"), 'A');
        assert_eq!(task.to_string(), "(A) Learn Rust +project");
    }

    #[test]
    fn test_task_to_str_completed() {
        let mut task = Task::new(String::from("Learn Rust"), String::from("project"), 'A');
        task.complete();
        assert_eq!(task.to_string(), "x (A) Learn Rust +project");
    }
}
