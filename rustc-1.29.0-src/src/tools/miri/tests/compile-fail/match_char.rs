// ignore-test FIXME: we are not checking these things on match any more?

fn main() {
    assert!(std::char::from_u32(-1_i32 as u32).is_none());
    match unsafe { std::mem::transmute::<i32, char>(-1) } { //~ ERROR constant evaluation error
        //~^ NOTE tried to interpret an invalid 32-bit value as a char: 4294967295
        'a' => {},
        'b' => {},
        _ => {},
    }
}
