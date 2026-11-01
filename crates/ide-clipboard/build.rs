fn main() {
    cc::Build::new()
        .cpp(true)
        .std("c++17")
        .file("src/clipboard_codec.cpp")
        .compile("ide_clipboard_codec");
    println!("cargo:rerun-if-changed=src/clipboard_codec.cpp");
}