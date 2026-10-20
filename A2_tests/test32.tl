fn main(): int {

    let x: int = 32;
    if (true) {
        if (true) {
            if (true) {
                if (true && false) {
                    return 6;
                }
                else {
                    let y: int = 10;
                    return x + y;
                }
            }
        }
    }
}