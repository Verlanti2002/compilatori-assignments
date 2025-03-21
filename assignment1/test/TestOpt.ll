; ModuleID = 'test/Test.ll'
source_filename = "Test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [15 x i8] c"Risultato: %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [26 x i8] c"Risultato per x = %d: %d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @algebraic_identity(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %5 = load i32, ptr %2, align 4
  store i32 %5, ptr %3, align 4
  %6 = load i32, ptr %2, align 4
  store i32 %6, ptr %4, align 4
  %7 = load i32, ptr %3, align 4
  %8 = load i32, ptr %4, align 4
  %9 = add nsw i32 %7, %8
  ret i32 %9
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @strength_reduction(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %8 = load i32, ptr %2, align 4
  %9 = shl i32 %8, 3
  store i32 %9, ptr %3, align 4
  %10 = load i32, ptr %2, align 4
  %11 = shl i32 %10, 4
  %12 = sub i32 %11, %10
  store i32 %12, ptr %4, align 4
  %13 = load i32, ptr %2, align 4
  %14 = shl i32 %13, 4
  %15 = add i32 %14, %13
  store i32 %15, ptr %5, align 4
  %16 = load i32, ptr %2, align 4
  %17 = shl i32 %16, 2
  store i32 %17, ptr %6, align 4
  %18 = load i32, ptr %2, align 4
  %19 = shl i32 %18, 4
  %20 = add i32 %19, %18
  store i32 %20, ptr %7, align 4
  %21 = load i32, ptr %3, align 4
  %22 = load i32, ptr %4, align 4
  %23 = add nsw i32 %21, %22
  %24 = load i32, ptr %5, align 4
  %25 = add nsw i32 %23, %24
  %26 = load i32, ptr %6, align 4
  %27 = add nsw i32 %25, %26
  %28 = load i32, ptr %7, align 4
  %29 = add nsw i32 %27, %28
  ret i32 %29
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @strength_reduction2(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %6 = load i32, ptr %2, align 4
  %7 = lshr i32 %6, 3
  store i32 %7, ptr %3, align 4
  %8 = load i32, ptr %2, align 4
  %9 = sdiv i32 %8, 10
  store i32 %9, ptr %4, align 4
  %10 = load i32, ptr %2, align 4
  %11 = lshr i32 %10, 4
  store i32 %11, ptr %5, align 4
  %12 = load i32, ptr %3, align 4
  %13 = load i32, ptr %4, align 4
  %14 = add nsw i32 %12, %13
  %15 = load i32, ptr %5, align 4
  %16 = add nsw i32 %14, %15
  ret i32 %16
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %4 = call i32 @algebraic_identity(i32 noundef 10)
  store i32 %4, ptr %2, align 4
  %5 = load i32, ptr %2, align 4
  %6 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %5)
  store i32 10, ptr %3, align 4
  %7 = load i32, ptr %3, align 4
  %8 = call i32 @strength_reduction(i32 noundef %7)
  store i32 %8, ptr %2, align 4
  %9 = load i32, ptr %3, align 4
  %10 = load i32, ptr %2, align 4
  %11 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %9, i32 noundef %10)
  store i32 10, ptr %3, align 4
  %12 = load i32, ptr %3, align 4
  %13 = call i32 @strength_reduction2(i32 noundef %12)
  store i32 %13, ptr %2, align 4
  %14 = load i32, ptr %3, align 4
  %15 = load i32, ptr %2, align 4
  %16 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %14, i32 noundef %15)
  ret i32 0
}

declare i32 @printf(ptr noundef, ...) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Debian clang version 19.1.4 (1~deb12u1)"}
