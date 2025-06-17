; ModuleID = 'test/Test.c'
source_filename = "test/Test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@N = dso_local constant i32 10, align 4
@.str = private unnamed_addr constant [24 x i8] c"Separazione tra i loop\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_all_properties_ok() #0 {
  %1 = alloca ptr, align 8
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = call ptr @llvm.stacksave.p0()
  store ptr %4, ptr %1, align 8
  %5 = alloca i32, i64 10, align 16
  %6 = alloca i32, i64 10, align 16
  %7 = alloca i32, i64 10, align 16
  %8 = alloca i32, i64 10, align 16
  store i32 0, ptr %2, align 4
  br label %9

9:                                                ; preds = %25, %0
  %10 = load i32, ptr %2, align 4
  %11 = icmp slt i32 %10, 10
  br i1 %11, label %12, label %28

12:                                               ; preds = %9
  %13 = load i32, ptr %2, align 4
  %14 = sext i32 %13 to i64
  %15 = getelementptr inbounds i32, ptr %5, i64 %14
  %16 = load i32, ptr %15, align 4
  %17 = load i32, ptr %2, align 4
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds i32, ptr %6, i64 %18
  %20 = load i32, ptr %19, align 4
  %21 = add nsw i32 %16, %20
  %22 = load i32, ptr %2, align 4
  %23 = sext i32 %22 to i64
  %24 = getelementptr inbounds i32, ptr %7, i64 %23
  store i32 %21, ptr %24, align 4
  br label %25

25:                                               ; preds = %12
  %26 = load i32, ptr %2, align 4
  %27 = add nsw i32 %26, 1
  store i32 %27, ptr %2, align 4
  br label %9, !llvm.loop !6

28:                                               ; preds = %9
  store i32 0, ptr %3, align 4
  br label %29

29:                                               ; preds = %41, %28
  %30 = load i32, ptr %3, align 4
  %31 = icmp slt i32 %30, 10
  br i1 %31, label %32, label %44

32:                                               ; preds = %29
  %33 = load i32, ptr %3, align 4
  %34 = sext i32 %33 to i64
  %35 = getelementptr inbounds i32, ptr %7, i64 %34
  %36 = load i32, ptr %35, align 4
  %37 = mul nsw i32 %36, 2
  %38 = load i32, ptr %3, align 4
  %39 = sext i32 %38 to i64
  %40 = getelementptr inbounds i32, ptr %8, i64 %39
  store i32 %37, ptr %40, align 4
  br label %41

41:                                               ; preds = %32
  %42 = load i32, ptr %3, align 4
  %43 = add nsw i32 %42, 1
  store i32 %43, ptr %3, align 4
  br label %29, !llvm.loop !8

44:                                               ; preds = %29
  %45 = load ptr, ptr %1, align 8
  call void @llvm.stackrestore.p0(ptr %45)
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare ptr @llvm.stacksave.p0() #1

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.stackrestore.p0(ptr) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_not_adjacent() #0 {
  %1 = alloca ptr, align 8
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = call ptr @llvm.stacksave.p0()
  store ptr %4, ptr %1, align 8
  %5 = alloca i32, i64 10, align 16
  %6 = alloca i32, i64 10, align 16
  %7 = alloca i32, i64 10, align 16
  %8 = alloca i32, i64 10, align 16
  store i32 0, ptr %2, align 4
  br label %9

9:                                                ; preds = %25, %0
  %10 = load i32, ptr %2, align 4
  %11 = icmp slt i32 %10, 10
  br i1 %11, label %12, label %28

12:                                               ; preds = %9
  %13 = load i32, ptr %2, align 4
  %14 = sext i32 %13 to i64
  %15 = getelementptr inbounds i32, ptr %5, i64 %14
  %16 = load i32, ptr %15, align 4
  %17 = load i32, ptr %2, align 4
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds i32, ptr %6, i64 %18
  %20 = load i32, ptr %19, align 4
  %21 = add nsw i32 %16, %20
  %22 = load i32, ptr %2, align 4
  %23 = sext i32 %22 to i64
  %24 = getelementptr inbounds i32, ptr %7, i64 %23
  store i32 %21, ptr %24, align 4
  br label %25

25:                                               ; preds = %12
  %26 = load i32, ptr %2, align 4
  %27 = add nsw i32 %26, 1
  store i32 %27, ptr %2, align 4
  br label %9, !llvm.loop !9

28:                                               ; preds = %9
  %29 = call i32 (ptr, ...) @printf(ptr noundef @.str)
  store i32 0, ptr %3, align 4
  br label %30

30:                                               ; preds = %42, %28
  %31 = load i32, ptr %3, align 4
  %32 = icmp slt i32 %31, 10
  br i1 %32, label %33, label %45

33:                                               ; preds = %30
  %34 = load i32, ptr %3, align 4
  %35 = sext i32 %34 to i64
  %36 = getelementptr inbounds i32, ptr %7, i64 %35
  %37 = load i32, ptr %36, align 4
  %38 = mul nsw i32 %37, 2
  %39 = load i32, ptr %3, align 4
  %40 = sext i32 %39 to i64
  %41 = getelementptr inbounds i32, ptr %8, i64 %40
  store i32 %38, ptr %41, align 4
  br label %42

42:                                               ; preds = %33
  %43 = load i32, ptr %3, align 4
  %44 = add nsw i32 %43, 1
  store i32 %44, ptr %3, align 4
  br label %30, !llvm.loop !10

45:                                               ; preds = %30
  %46 = load ptr, ptr %1, align 8
  call void @llvm.stackrestore.p0(ptr %46)
  ret void
}

declare i32 @printf(ptr noundef, ...) #2

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_control_flow_not_equivalent() #0 {
  %1 = alloca ptr, align 8
  %2 = alloca i8, align 1
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = call ptr @llvm.stacksave.p0()
  store ptr %5, ptr %1, align 8
  %6 = alloca i32, i64 10, align 16
  %7 = alloca i32, i64 10, align 16
  store i8 1, ptr %2, align 1
  %8 = load i8, ptr %2, align 1
  %9 = trunc i8 %8 to i1
  br i1 %9, label %10, label %23

10:                                               ; preds = %0
  store i32 0, ptr %3, align 4
  br label %11

11:                                               ; preds = %19, %10
  %12 = load i32, ptr %3, align 4
  %13 = icmp slt i32 %12, 10
  br i1 %13, label %14, label %22

14:                                               ; preds = %11
  %15 = load i32, ptr %3, align 4
  %16 = load i32, ptr %3, align 4
  %17 = sext i32 %16 to i64
  %18 = getelementptr inbounds i32, ptr %6, i64 %17
  store i32 %15, ptr %18, align 4
  br label %19

19:                                               ; preds = %14
  %20 = load i32, ptr %3, align 4
  %21 = add nsw i32 %20, 1
  store i32 %21, ptr %3, align 4
  br label %11, !llvm.loop !11

22:                                               ; preds = %11
  br label %23

23:                                               ; preds = %22, %0
  store i32 0, ptr %4, align 4
  br label %24

24:                                               ; preds = %36, %23
  %25 = load i32, ptr %4, align 4
  %26 = icmp slt i32 %25, 10
  br i1 %26, label %27, label %39

27:                                               ; preds = %24
  %28 = load i32, ptr %4, align 4
  %29 = sext i32 %28 to i64
  %30 = getelementptr inbounds i32, ptr %6, i64 %29
  %31 = load i32, ptr %30, align 4
  %32 = mul nsw i32 %31, 2
  %33 = load i32, ptr %4, align 4
  %34 = sext i32 %33 to i64
  %35 = getelementptr inbounds i32, ptr %7, i64 %34
  store i32 %32, ptr %35, align 4
  br label %36

36:                                               ; preds = %27
  %37 = load i32, ptr %4, align 4
  %38 = add nsw i32 %37, 1
  store i32 %38, ptr %4, align 4
  br label %24, !llvm.loop !12

39:                                               ; preds = %24
  %40 = load ptr, ptr %1, align 8
  call void @llvm.stackrestore.p0(ptr %40)
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_different_trip_count() #0 {
  %1 = alloca ptr, align 8
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = call ptr @llvm.stacksave.p0()
  store ptr %4, ptr %1, align 8
  %5 = alloca i32, i64 10, align 16
  %6 = alloca i32, i64 10, align 16
  %7 = alloca i32, i64 10, align 16
  %8 = alloca i32, i64 10, align 16
  store i32 0, ptr %2, align 4
  br label %9

9:                                                ; preds = %25, %0
  %10 = load i32, ptr %2, align 4
  %11 = icmp slt i32 %10, 10
  br i1 %11, label %12, label %28

12:                                               ; preds = %9
  %13 = load i32, ptr %2, align 4
  %14 = sext i32 %13 to i64
  %15 = getelementptr inbounds i32, ptr %5, i64 %14
  %16 = load i32, ptr %15, align 4
  %17 = load i32, ptr %2, align 4
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds i32, ptr %6, i64 %18
  %20 = load i32, ptr %19, align 4
  %21 = add nsw i32 %16, %20
  %22 = load i32, ptr %2, align 4
  %23 = sext i32 %22 to i64
  %24 = getelementptr inbounds i32, ptr %7, i64 %23
  store i32 %21, ptr %24, align 4
  br label %25

25:                                               ; preds = %12
  %26 = load i32, ptr %2, align 4
  %27 = add nsw i32 %26, 1
  store i32 %27, ptr %2, align 4
  br label %9, !llvm.loop !13

28:                                               ; preds = %9
  store i32 1, ptr %3, align 4
  br label %29

29:                                               ; preds = %41, %28
  %30 = load i32, ptr %3, align 4
  %31 = icmp slt i32 %30, 10
  br i1 %31, label %32, label %44

32:                                               ; preds = %29
  %33 = load i32, ptr %3, align 4
  %34 = sext i32 %33 to i64
  %35 = getelementptr inbounds i32, ptr %7, i64 %34
  %36 = load i32, ptr %35, align 4
  %37 = mul nsw i32 %36, 2
  %38 = load i32, ptr %3, align 4
  %39 = sext i32 %38 to i64
  %40 = getelementptr inbounds i32, ptr %8, i64 %39
  store i32 %37, ptr %40, align 4
  br label %41

41:                                               ; preds = %32
  %42 = load i32, ptr %3, align 4
  %43 = add nsw i32 %42, 1
  store i32 %43, ptr %3, align 4
  br label %29, !llvm.loop !14

44:                                               ; preds = %29
  %45 = load ptr, ptr %1, align 8
  call void @llvm.stackrestore.p0(ptr %45)
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_negative_dependence() #0 {
  %1 = alloca ptr, align 8
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = call ptr @llvm.stacksave.p0()
  store ptr %4, ptr %1, align 8
  %5 = alloca i32, i64 10, align 16
  %6 = alloca i32, i64 10, align 16
  store i32 0, ptr %2, align 4
  br label %7

7:                                                ; preds = %18, %0
  %8 = load i32, ptr %2, align 4
  %9 = icmp slt i32 %8, 10
  br i1 %9, label %10, label %21

10:                                               ; preds = %7
  %11 = load i32, ptr %2, align 4
  %12 = sext i32 %11 to i64
  %13 = getelementptr inbounds i32, ptr %5, i64 %12
  %14 = load i32, ptr %13, align 4
  %15 = load i32, ptr %2, align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds i32, ptr %6, i64 %16
  store i32 %14, ptr %17, align 4
  br label %18

18:                                               ; preds = %10
  %19 = load i32, ptr %2, align 4
  %20 = add nsw i32 %19, 1
  store i32 %20, ptr %2, align 4
  br label %7, !llvm.loop !15

21:                                               ; preds = %7
  store i32 0, ptr %3, align 4
  br label %22

22:                                               ; preds = %35, %21
  %23 = load i32, ptr %3, align 4
  %24 = icmp slt i32 %23, 10
  br i1 %24, label %25, label %38

25:                                               ; preds = %22
  %26 = load i32, ptr %3, align 4
  %27 = add nsw i32 %26, 3
  %28 = sext i32 %27 to i64
  %29 = getelementptr inbounds i32, ptr %6, i64 %28
  %30 = load i32, ptr %29, align 4
  %31 = add nsw i32 %30, 1
  %32 = load i32, ptr %3, align 4
  %33 = sext i32 %32 to i64
  %34 = getelementptr inbounds i32, ptr %6, i64 %33
  store i32 %31, ptr %34, align 4
  br label %35

35:                                               ; preds = %25
  %36 = load i32, ptr %3, align 4
  %37 = add nsw i32 %36, 1
  store i32 %37, ptr %3, align 4
  br label %22, !llvm.loop !16

38:                                               ; preds = %22
  %39 = load ptr, ptr %1, align 8
  call void @llvm.stackrestore.p0(ptr %39)
  ret void
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind willreturn }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

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
!8 = distinct !{!8, !7}
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
!11 = distinct !{!11, !7}
!12 = distinct !{!12, !7}
!13 = distinct !{!13, !7}
!14 = distinct !{!14, !7}
!15 = distinct !{!15, !7}
!16 = distinct !{!16, !7}
