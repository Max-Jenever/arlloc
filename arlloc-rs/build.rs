fn main() {
    let c_src_dir = "c-src";

    // 2. Инструктируем Cargo пересобирать крейт при изменении C-файлов.
    // Это обеспечивает корректную инкрементальную сборку.
    println!("cargo:rerun-if-changed={}/arlloc.c", c_src_dir);
    println!("cargo:rerun-if-changed={}/arlloc.h", c_src_dir);

    let mut build = cc::Build::new();

    build.file(format!("{}/arlloc.c", c_src_dir));

    build.flag_if_supported("-Wall");
    build.flag_if_supported("-Wextra");
    build.flag_if_supported("-Wpedantic");


    // 6. Компилируем код в статическую библиотеку с именем "arlloc".
    // Крейт cc автоматически создаст libarlloc.a (Unix) или arlloc.lib (Windows)
    // и передаст Cargo необходимые инструкции для линковки (rustc-link-lib и rustc-link-search).
    build.compile("arlloc");
}
