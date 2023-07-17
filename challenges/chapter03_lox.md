## 1. Write some sample lox programs.

```
class List {
  init(data, next) {
    this.data = data;
    this.next = next;
  }

  map(function) {
    var data = function(this.data);
    var next;
    if (this.next != nil) next = this.next.map(function);
    return List(data, next);
  }

  display() {
    var list = this;
    while (list != nil) {
      print(list.data);
      list = list.next;
    }
  }
}

var list = List(1, List(2, List(3, List(4, nil))));
list.display();

fun double(n) { return n * 2; }
list = list.map(double);
list.display();
```

## 2. Questions about Lox syntax.

- What happens if you access a global variable in a function before it is defined?
- What does it mean to say something is "an error"? Runtime error? Compile time?
- What kind of expressions are allowed when a superclass is specified?
- What happens if you declare two classes or functions with the same name?
- Can a class inherit from something that isn't a class?
- Can you reassign to the name that is bound by a class or function declaration?

## 3. What features are missing in Lox?

- Lists/arrays. It's possible to build linked lists, but there's no way to create a data structure that stores a contiguous series of values that can be accessed in constant time in user code. That has to be baked into the language or core library.
- Some mechanism for handling runtime errors along the lines of exception handling.
- No `break` or `continue` for loops.
- No `switch`.
