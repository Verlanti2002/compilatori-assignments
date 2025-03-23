; ModuleID = 'test/Test.ll'
source_filename = "Test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [15 x i8] c"Risultato: %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [26 x i8] c"Risultato per x = %d: %d\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @algebraic_identity(i32 noundef %0) #0 {
  %2 = add nsw i32 %0, %0
  ret i32 %2
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @strength_reduction(i32 noundef %0) #0 {
  %2 = shl i32 %0, 3
  %3 = shl i32 %0, 4
  %4 = sub i32 %3, 15
  %5 = shl i32 %0, 4
  %6 = add i32 %5, 17
  %7 = shl i32 %0, 2
  %8 = shl i32 %0, 4
  %9 = add i32 %8, 17
  %10 = add nsw i32 %2, %4
  %11 = add nsw i32 %10, %6
  %12 = add nsw i32 %11, %7
  %13 = add nsw i32 %12, %9
  ret i32 %13
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @strength_reduction2(i32 noundef %0) #0 {
  %2 = lshr i32 %0, 3
  %3 = sdiv i32 %0, 10
  %4 = lshr i32 %0, 4
  %5 = add nsw i32 %2, %3
  %6 = add nsw i32 %5, %4
  ret i32 %6
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @multi_instruction(i32 noundef %0, i32 noundef %1) #0 {
  %3 = add nsw i32 %0, 1
  %4 = sub nsw i32 %1, 1
  %5 = add nsw i32 %0, %1
  ret i32 %5
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @algebraic_identity(i32 noundef 10)
  %2 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %1)
  %3 = call i32 @strength_reduction(i32 noundef 10)
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef 10, i32 noundef %3)
  %5 = call i32 @strength_reduction2(i32 noundef 10)
  %6 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef 10, i32 noundef %5)
  %7 = call i32 @multi_instruction(i32 noundef 10, i32 noundef 10)
  %8 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef 10, i32 noundef %7)
  ret i32 0
}

declare i32 @printf(ptr noundef, ...) #1

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Debian clang version 19.1.4 (1~deb12u1)"}
