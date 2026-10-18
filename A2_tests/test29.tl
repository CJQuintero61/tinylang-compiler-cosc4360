fn main(): int {
    let x: int = 29;

    if (true) {
        if (true) {
            if (true) {
                if (false) {
                    return x;
                }
                else {
                    let y: int = 1;
                    return x * y;
                }
            }
        }
    }
}