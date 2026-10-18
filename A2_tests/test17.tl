fn main(): int {
    let y: int = 10;

    if (false) {
        let x: int = 20;
        return y + x;
    }
    else {
        return y;
    }
}