#[derive(Clone)]
pub struct Task {
    pub name: String,
    pub project: String,
    completed: bool,
    priority: char,
}

impl Task {
    pub fn to_str(&self) -> String {
        let mut s = String::new();
        if self.completed {
            s.push('x');
            s.push(' ');
        }
        s.push('(');
        s.push(self.priority);
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

    #[test]
    fn test_task_to_str_no_project() {
        let task = Task::new(String::from("Learn Rust"), String::from(""), 'A');
        assert_eq!(task.to_str(), "(A) Learn Rust");
    }

    #[test]
    fn test_task_to_str() {
        let task = Task::new(String::from("Learn Rust"), String::from("project"), 'A');
        assert_eq!(task.to_str(), "(A) Learn Rust +project");
    }

    #[test]
    fn test_task_to_str_completed() {
        let mut task = Task::new(String::from("Learn Rust"), String::from("project"), 'A');
        task.complete();
        assert_eq!(task.to_str(), "x (A) Learn Rust +project");
    }
}
