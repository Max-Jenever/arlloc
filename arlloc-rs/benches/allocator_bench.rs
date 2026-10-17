use arlloc::Arena;
use bumpalo::Bump;
use criterion::{Criterion, black_box, criterion_group, criterion_main};

#[derive(Clone, Copy)]
struct TestStruct {
    a: u64,
    b: u64,
    c: u64,
    d: u64,
    e: u64,
    f: u64,
    g: u64,
    h: u64,
}

fn bench_arlloc(c: &mut Criterion) {
    let mut group = c.benchmark_group("arlloc");

    group.bench_function("alloc_single", |b| {
        let arena = Arena::new(4096, 4096).expect("Failed to create arena");
        b.iter(|| {
            let _val = arena
                .alloc(black_box(TestStruct {
                    a: 1,
                    b: 2,
                    c: 3,
                    d: 4,
                    e: 5,
                    f: 6,
                    g: 7,
                    h: 8,
                }))
                .expect("Alloc failed");
        });
    });

    // Чистая скорость аллокации без учета создания/уничтожения арены
    group.bench_function("bulk_alloc_only_10k", |b| {
        let arena = Arena::new(65536, 65536).expect("Failed to create arena");
        b.iter(|| {
            for _ in 0..10_000 {
                let _val = arena
                    .alloc(black_box(TestStruct {
                        a: 1,
                        b: 2,
                        c: 3,
                        d: 4,
                        e: 5,
                        f: 6,
                        g: 7,
                        h: 8,
                    }))
                    .expect("Alloc failed");
            }
        });
    });

    group.bench_function("lifecycle_bulk_10k", |b| {
        b.iter(|| {
            let arena = Arena::new(65536, 65536).expect("Failed to create arena");
            for _ in 0..10_000 {
                let _val = arena
                    .alloc(black_box(TestStruct {
                        a: 1,
                        b: 2,
                        c: 3,
                        d: 4,
                        e: 5,
                        f: 6,
                        g: 7,
                        h: 8,
                    }))
                    .expect("Alloc failed");
            }
        });
    });
    group.finish();
}

fn bench_bumpalo(c: &mut Criterion) {
    let mut group = c.benchmark_group("bumpalo");

    group.bench_function("alloc_single", |b| {
        let bump = Bump::with_capacity(4096);
        b.iter(|| {
            let _val = bump.alloc(black_box(TestStruct {
                a: 1,
                b: 2,
                c: 3,
                d: 4,
                e: 5,
                f: 6,
                g: 7,
                h: 8,
            }));
        });
    });

    group.bench_function("bulk_alloc_only_10k", |b| {
        let bump = Bump::with_capacity(65536);
        b.iter(|| {
            for _ in 0..10_000 {
                let _val = bump.alloc(black_box(TestStruct {
                    a: 1,
                    b: 2,
                    c: 3,
                    d: 4,
                    e: 5,
                    f: 6,
                    g: 7,
                    h: 8,
                }));
            }
        });
    });

    group.bench_function("lifecycle_bulk_10k", |b| {
        b.iter(|| {
            let bump = Bump::with_capacity(65536);
            for _ in 0..10_000 {
                let _val = bump.alloc(black_box(TestStruct {
                    a: 1,
                    b: 2,
                    c: 3,
                    d: 4,
                    e: 5,
                    f: 6,
                    g: 7,
                    h: 8,
                }));
            }
        });
    });
    group.finish();
}

criterion_group!(benches, bench_arlloc, bench_bumpalo);
criterion_main!(benches);
