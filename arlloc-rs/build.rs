fn main(){

    println!("cargo:rustc-link-search=native=../C-Arena/src");

    println!("cargo:rustc-link-lib=static=arlloc");

    println!("cargo:rerun-if-changed=../C-Arena/src/arlloc.c");
    println!("cargo:rerun-if-changed=../C-Arena/src/arlloc.h");
}
