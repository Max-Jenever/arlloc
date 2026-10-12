# ============================================================================
# arlloc - Arena Memory Allocator
# Корневой Makefile для оркестрации сборки C и Rust компонентов
# ============================================================================

# Директории компонентов (C-код находится в src внутри C-Arena)
C_DIR := C-Arena/src
RS_DIR := arlloc-rs

# Директория для выходной документации
DOCS_DIR := docs

# Цели по умолчанию
.PHONY: all clean docs docs-c docs-rs test help

all: build-c build-rs

# ============================================================================
# Сборка компонентов
# ============================================================================

.PHONY: build-c build-rs

build-c:
	@echo "[BUILD] C library..."
	@$(MAKE) -C $(C_DIR)
	@echo "[BUILD] C library completed."

build-rs:
	@echo "[BUILD] Rust wrapper..."
	@cd $(RS_DIR) && cargo build --release
	@echo "[BUILD] Rust wrapper completed."

# ============================================================================
# Тестирование
# ============================================================================

.PHONY: test-c test-rs test

test: test-c test-rs
	@echo "[TEST] All tests passed."

test-c:
	@echo "[TEST] C tests..."
	@$(MAKE) -C $(C_DIR) test
	@echo "[TEST] C tests completed."

test-rs:
	@echo "[TEST] Rust tests..."
	@cd $(RS_DIR) && cargo test
	@echo "[TEST] Rust tests completed."

# ============================================================================
# Генерация документации
# ============================================================================

docs: docs-c docs-rs
	@echo "[DOCS] Documentation generated successfully."
	@echo "[DOCS] C documentation: file://$(PWD)/$(DOCS_DIR)/c/index.html"
	@echo "[DOCS] Rust documentation: file://$(PWD)/$(DOCS_DIR)/rust/arlloc_rs/index.html"

docs-c: $(DOCS_DIR)/c
	@echo "[DOCS] Generating C documentation..."
	@cd C-Arena && doxygen Doxyfile
	@mkdir -p $(DOCS_DIR)/c
	@cp -r $(C_DIR)/html/* $(DOCS_DIR)/c/ 2>/dev/null || true
	@echo "[DOCS] C documentation completed."

docs-rs: $(DOCS_DIR)/rust
	@echo "[DOCS] Generating Rust documentation..."
	@cd $(RS_DIR) && cargo doc --no-deps
	@mkdir -p $(DOCS_DIR)/rust
	@cp -r $(RS_DIR)/target/doc/* $(DOCS_DIR)/rust/ 2>/dev/null || true
	@echo "[DOCS] Rust documentation completed."

$(DOCS_DIR)/c:
	@mkdir -p $(DOCS_DIR)/c

$(DOCS_DIR)/rust:
	@mkdir -p $(DOCS_DIR)/rust

# ============================================================================
# Очистка
# ============================================================================

clean:
	@echo "[CLEAN] Removing build artifacts..."
	@$(MAKE) -C $(C_DIR) clean
	@cd $(RS_DIR) && cargo clean
	@rm -rf $(DOCS_DIR)
	@echo "[CLEAN] Cleanup completed."

# ============================================================================
# Справка
# ============================================================================

help:
	@echo "arlloc - Arena Memory Allocator"
	@echo ""
	@echo "Available targets:"
	@echo "  all        - Собрать C и Rust компоненты (по умолчанию)"
	@echo "  build-c    - Собрать только C библиотеку"
	@echo "  build-rs   - Собрать только Rust обертку"
	@echo "  test       - Запустить все тесты (C и Rust)"
	@echo "  test-c     - Запустить только C тесты"
	@echo "  test-rs    - Запустить только Rust тесты"
	@echo "  docs       - Сгенерировать документацию для обоих компонентов"
	@echo "  docs-c     - Сгенерировать документацию только для C"
	@echo "  docs-rs    - Сгенерировать документацию только для Rust"
	@echo "  clean      - Удалить все артефакты сборки и документацию"
	@echo "  help       - Показать эту справку"
