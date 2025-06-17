; ModuleID = 'test/Test.c'
source_filename = "test/Test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @test() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 5, ptr %1, align 4
  store i32 10, ptr %2, align 4
  store i32 0, ptr %3, align 4
  store i32 0, ptr %4, align 4
  br label %7

7:                                                ; preds = %23, %0
  %8 = load i32, ptr %1, align 4
  %9 = load i32, ptr %2, align 4
  %10 = add nsw i32 %8, %9
  store i32 %10, ptr %6, align 4
  %11 = load i32, ptr %3, align 4
  %12 = icmp sgt i32 %11, 5
  br i1 %12, label %13, label %17

13:                                               ; preds = %7
  %14 = load i32, ptr %1, align 4
  %15 = load i32, ptr %2, align 4
  %16 = add nsw i32 %14, %15
  store i32 %16, ptr %4, align 4
  br label %17

17:                                               ; preds = %13, %7
  %18 = load i32, ptr %5, align 4
  %19 = load i32, ptr %3, align 4
  %20 = add nsw i32 %18, %19
  store i32 %20, ptr %5, align 4
  %21 = load i32, ptr %3, align 4
  %22 = add nsw i32 %21, 1
  store i32 %22, ptr %3, align 4
  br label %23

23:                                               ; preds = %17
  %24 = load i32, ptr %3, align 4
  %25 = icmp slt i32 %24, 10
  br i1 %25, label %7, label %26, !llvm.loop !6

26:                                               ; preds = %23
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  call void @test()
  ret i32 0
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Debian clang version 19.1.4 (1~deb12u1)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
