// ignore-test FIXME: we are not making these statics read-only any more?

fn main() {
    let x = &1; // the `&1` is promoted to a constant, but it used to be that only the pointer is marked static, not the pointee
    let y = unsafe { &mut *(x as *const i32 as *mut i32) };
    *y = 42;  //~ ERROR constant evaluation error
    //~^ NOTE tried to modify constant memory
    assert_eq!(*x, 42);
}
