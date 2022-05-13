pub struct Task {
    pub name: String,
    pub project: String,
    completed: bool,
    priority: char,
}

impl Task {
    pub fn new(name: String, project: String, priority: char) -> Task {
        Task {
            name: name,
            project: project,
            completed: false,
            priority: priority,
        }
    }

    pub fn complete(&mut self) {
        self.completed = true;
    }

    pub fn reopen(&mut self) {
        self.completed = false;
    }

    pub fn is_complete(&self) -> bool {
        self.completed
    }

    pub fn get_priority(&self) -> char {
        self.priority
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_task_new() {
        let task = Task::new(String::from("Learn Rust"), String::from("Learn Rust"), 'A');
        assert_eq!(task.name, "Learn Rust");
        assert_eq!(task.project, "Learn Rust");
        assert_eq!(task.priority, 'A');
    }

    #[test]
    fn test_task_complete() {
        let mut task = Task::new(String::from("Learn Rust"), String::from("Learn Rust"), 'A');
        task.complete();
        assert_eq!(task.completed, true);
    }

    #[test]
    fn test_task_reopen() {
        let mut task = Task::new(String::from("Learn Rust"), String::from("Learn Rust"), 'A');
        task.complete();
        task.reopen();
        assert_eq!(task.completed, false);
    }

    #[test]
    fn test_task_is_complete() {
        let task = Task::new(String::from("Learn Rust"), String::from("Learn Rust"), 'A');
        assert_eq!(task.is_complete(), false);
    }

    #[test]
    fn test_task_get_priority() {
        let task = Task::new(String::from("Learn Rust"), String::from("Learn Rust"), 'A');
        assert_eq!(task.get_priority(), 'A');
    }
}
