use std::env;
use std::path::PathBuf;
use std::process::Command;
use std::fs;

fn main() {
    // 1. Получаем абсолютный путь к директории arlloc-rs (где лежит Cargo.toml)
    let manifest_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    
    // 2. Поднимаемся на уровень выше до корня репозитория
    let workspace_root = manifest_dir.parent().expect("Не удалось определить корень воркспейса");
    
    // 3. Формируем абсолютные пути к исходникам C и целевой папке сборки
    let c_src_dir = workspace_root.join("C-Arena").join("src");
    let c_build_dir = workspace_root.join("C-Arena").join("build");

    // 4. Гарантируем существование папки build до запуска make 
    // (дублирует логику Makefile для надежности при запуске из Rust)
    fs::create_dir_all(&c_build_dir).expect("Не удалось создать директорию C-Arena/build");

    // 5. Отслеживаем изменения только в исходных файлах и Makefile.
    // Cargo перезапустит этот скрипт, если изменится любой из них.
    println!("cargo:rerun-if-changed={}", c_src_dir.join("arlloc.c").display());
    println!("cargo:rerun-if-changed={}", c_src_dir.join("arlloc.h").display());
    println!("cargo:rerun-if-changed={}", c_src_dir.join("Makefile").display());

    println!("cargo:warning=Компиляция C-библиотеки arlloc...");
    
    // 6. Запускаем make в директории src. 
    // Относительный путь ../build в вашем Makefile корректно разрешится именно в c_build_dir.
    let status = Command::new("make")
        .current_dir(&c_src_dir)
        .arg("all") // Явно указываем цель, чтобы не собирать тесты при обычной сборке Rust
        .status()
        .expect("Не удалось выполнить команду 'make'. Убедитесь, что он установлен в системе.");

    if !status.success() {
        panic!("Сборка C-библиотеки завершилась с ошибкой. Проверьте вывод компилятора выше.");
    }

    // 7. Инструктируем Cargo искать библиотеку в папке build
    println!("cargo:rustc-link-search=native={}", c_build_dir.display());
    
    // 8. Инструктируем Cargo слинковать статическую библиотеку libarlloc.a
    println!("cargo:rustc-link-lib=static=arlloc");
}
