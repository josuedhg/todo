use crate::Task;

pub trait Todo {
    fn add(&mut self, task: Task);
    fn remove(&mut self, index: usize);
    fn list(&mut self) -> Vec<Task>;
}
