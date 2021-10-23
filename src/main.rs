
struct Task {
    name: String,
    project: String,
    completed: bool,
    priority: char,
}

fn main() {
    let task = Task {
        name: String::from("Learn Rust"),
        project: String::from("Learn Rust"),
        completed: false,
        priority: 'A',
    };
    println!("{}", task.name);
    println!("{}", task.project);
    let completed_message = if task.completed {
        "Completed"
    } else {
        "Not completed"
    };
    println!("{}", completed_message);
    println!("{}", task.priority);
}
