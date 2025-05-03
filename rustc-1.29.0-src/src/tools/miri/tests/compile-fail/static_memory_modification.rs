// ignore-test FIXME: we are not making these statics read-only any more?
static X: usize = 5;

#[allow(mutable_transmutes)]
fn main() {
    unsafe {
        *std::mem::transmute::<&usize, &mut usize>(&X) = 6; //~ ERROR constant evaluation error
        //~^ NOTE tried to modify constant memory
        assert_eq!(X, 6);
    }
}
