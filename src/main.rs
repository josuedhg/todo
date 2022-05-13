use todo::Task;

fn main() {
    let task = Task::new(String::from("Learn Rust"), String::from("Learn Rust"), 'A');
    println!("{}", task.name);
    println!("{}", task.project);
    let completed_message = if task.is_complete() {
        "Completed"
    } else {
        "Not completed"
    };
    println!("{}", completed_message);
    println!("{}", task.get_priority());
}
