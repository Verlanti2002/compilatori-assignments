; ModuleID = 'test/Test.m2r.ll'
source_filename = "test/Test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@N = dso_local constant i32 10, align 4
@.str = private unnamed_addr constant [24 x i8] c"Separazione tra i loop\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_all_properties_ok() #0 {
  %1 = call ptr @llvm.stacksave.p0()
  %2 = alloca i32, i64 10, align 16
  %3 = alloca i32, i64 10, align 16
  %4 = alloca i32, i64 10, align 16
  %5 = alloca i32, i64 10, align 16
  br label %6

6:                                                ; preds = %13, %0
  %indvars.iv7 = phi i64 [ 0, %0 ], [ %indvars.iv.next, %13 ]
  %7 = getelementptr inbounds i32, ptr %2, i64 %indvars.iv7
  %8 = load i32, ptr %7, align 4
  %9 = getelementptr inbounds i32, ptr %3, i64 %indvars.iv7
  %10 = load i32, ptr %9, align 4
  %11 = add nsw i32 %8, %10
  %12 = getelementptr inbounds i32, ptr %4, i64 %indvars.iv7
  store i32 %11, ptr %12, align 4
  br label %15

13:                                               ; preds = %15
  %indvars.iv.next = add nuw nsw i64 %indvars.iv7, 1
  %exitcond = icmp ne i64 %indvars.iv.next, 10
  br i1 %exitcond, label %6, label %14, !llvm.loop !6

14:                                               ; preds = %13
  br label %15

15:                                               ; preds = %6, %20, %14
  %16 = getelementptr inbounds i32, ptr %4, i64 %indvars.iv7
  %17 = load i32, ptr %16, align 4
  %18 = mul nsw i32 %17, 2
  %19 = getelementptr inbounds i32, ptr %5, i64 %indvars.iv7
  store i32 %18, ptr %19, align 4
  br label %13

20:                                               ; No predecessors!
  %indvars.iv.next4 = add nuw nsw i64 %indvars.iv7, 1
  %exitcond6 = icmp ne i64 %indvars.iv.next4, 10
  br i1 %exitcond6, label %15, label %21, !llvm.loop !8

21:                                               ; preds = %20
  call void @llvm.stackrestore.p0(ptr %1)
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare ptr @llvm.stacksave.p0() #1

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.stackrestore.p0(ptr) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_not_adjacent() #0 {
  %1 = call ptr @llvm.stacksave.p0()
  %2 = alloca i32, i64 10, align 16
  %3 = alloca i32, i64 10, align 16
  %4 = alloca i32, i64 10, align 16
  %5 = alloca i32, i64 10, align 16
  br label %6

6:                                                ; preds = %13, %0
  %indvars.iv7 = phi i64 [ 0, %0 ], [ %indvars.iv.next, %13 ]
  %7 = getelementptr inbounds i32, ptr %2, i64 %indvars.iv7
  %8 = load i32, ptr %7, align 4
  %9 = getelementptr inbounds i32, ptr %3, i64 %indvars.iv7
  %10 = load i32, ptr %9, align 4
  %11 = add nsw i32 %8, %10
  %12 = getelementptr inbounds i32, ptr %4, i64 %indvars.iv7
  store i32 %11, ptr %12, align 4
  br label %13

13:                                               ; preds = %6
  %indvars.iv.next = add nuw nsw i64 %indvars.iv7, 1
  %exitcond = icmp ne i64 %indvars.iv.next, 10
  br i1 %exitcond, label %6, label %14, !llvm.loop !9

14:                                               ; preds = %13
  %15 = call i32 (ptr, ...) @printf(ptr noundef @.str)
  br label %16

16:                                               ; preds = %21, %14
  %indvars.iv38 = phi i64 [ 0, %14 ], [ %indvars.iv.next4, %21 ]
  %17 = getelementptr inbounds i32, ptr %4, i64 %indvars.iv38
  %18 = load i32, ptr %17, align 4
  %19 = mul nsw i32 %18, 2
  %20 = getelementptr inbounds i32, ptr %5, i64 %indvars.iv38
  store i32 %19, ptr %20, align 4
  br label %21

21:                                               ; preds = %16
  %indvars.iv.next4 = add nuw nsw i64 %indvars.iv38, 1
  %exitcond6 = icmp ne i64 %indvars.iv.next4, 10
  br i1 %exitcond6, label %16, label %22, !llvm.loop !10

22:                                               ; preds = %21
  call void @llvm.stackrestore.p0(ptr %1)
  ret void
}

declare i32 @printf(ptr noundef, ...) #2

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_control_flow_not_equivalent() #0 {
  %1 = call ptr @llvm.stacksave.p0()
  %2 = alloca i32, i64 10, align 16
  %3 = alloca i32, i64 10, align 16
  %4 = trunc i8 1 to i1
  br i1 %4, label %5, label %11

5:                                                ; preds = %0
  br label %6

6:                                                ; preds = %9, %5
  %indvars.iv7 = phi i64 [ 0, %5 ], [ %indvars.iv.next, %9 ]
  %7 = getelementptr inbounds i32, ptr %2, i64 %indvars.iv7
  %8 = trunc nuw nsw i64 %indvars.iv7 to i32
  store i32 %8, ptr %7, align 4
  br label %9

9:                                                ; preds = %6
  %indvars.iv.next = add nuw nsw i64 %indvars.iv7, 1
  %exitcond = icmp ne i64 %indvars.iv.next, 10
  br i1 %exitcond, label %6, label %10, !llvm.loop !11

10:                                               ; preds = %9
  br label %11

11:                                               ; preds = %10, %0
  br label %12

12:                                               ; preds = %17, %11
  %indvars.iv38 = phi i64 [ 0, %11 ], [ %indvars.iv.next4, %17 ]
  %13 = getelementptr inbounds i32, ptr %2, i64 %indvars.iv38
  %14 = load i32, ptr %13, align 4
  %15 = mul nsw i32 %14, 2
  %16 = getelementptr inbounds i32, ptr %3, i64 %indvars.iv38
  store i32 %15, ptr %16, align 4
  br label %17

17:                                               ; preds = %12
  %indvars.iv.next4 = add nuw nsw i64 %indvars.iv38, 1
  %exitcond6 = icmp ne i64 %indvars.iv.next4, 10
  br i1 %exitcond6, label %12, label %18, !llvm.loop !12

18:                                               ; preds = %17
  call void @llvm.stackrestore.p0(ptr %1)
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_different_trip_count() #0 {
  %1 = call ptr @llvm.stacksave.p0()
  %2 = alloca i32, i64 10, align 16
  %3 = alloca i32, i64 10, align 16
  %4 = alloca i32, i64 10, align 16
  %5 = alloca i32, i64 10, align 16
  br label %6

6:                                                ; preds = %13, %0
  %indvars.iv7 = phi i64 [ 0, %0 ], [ %indvars.iv.next, %13 ]
  %7 = getelementptr inbounds i32, ptr %2, i64 %indvars.iv7
  %8 = load i32, ptr %7, align 4
  %9 = getelementptr inbounds i32, ptr %3, i64 %indvars.iv7
  %10 = load i32, ptr %9, align 4
  %11 = add nsw i32 %8, %10
  %12 = getelementptr inbounds i32, ptr %4, i64 %indvars.iv7
  store i32 %11, ptr %12, align 4
  br label %13

13:                                               ; preds = %6
  %indvars.iv.next = add nuw nsw i64 %indvars.iv7, 1
  %exitcond = icmp ne i64 %indvars.iv.next, 10
  br i1 %exitcond, label %6, label %14, !llvm.loop !13

14:                                               ; preds = %13
  br label %15

15:                                               ; preds = %20, %14
  %indvars.iv38 = phi i64 [ 1, %14 ], [ %indvars.iv.next4, %20 ]
  %16 = getelementptr inbounds i32, ptr %4, i64 %indvars.iv38
  %17 = load i32, ptr %16, align 4
  %18 = mul nsw i32 %17, 2
  %19 = getelementptr inbounds i32, ptr %5, i64 %indvars.iv38
  store i32 %18, ptr %19, align 4
  br label %20

20:                                               ; preds = %15
  %indvars.iv.next4 = add nuw nsw i64 %indvars.iv38, 1
  %exitcond6 = icmp ne i64 %indvars.iv.next4, 10
  br i1 %exitcond6, label %15, label %21, !llvm.loop !14

21:                                               ; preds = %20
  call void @llvm.stackrestore.p0(ptr %1)
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @test_negative_dependence() #0 {
  %1 = call ptr @llvm.stacksave.p0()
  %2 = alloca i32, i64 10, align 16
  %3 = alloca i32, i64 10, align 16
  br label %4

4:                                                ; preds = %8, %0
  %indvars.iv7 = phi i64 [ 0, %0 ], [ %indvars.iv.next, %8 ]
  %5 = getelementptr inbounds i32, ptr %2, i64 %indvars.iv7
  %6 = load i32, ptr %5, align 4
  %7 = getelementptr inbounds i32, ptr %3, i64 %indvars.iv7
  store i32 %6, ptr %7, align 4
  br label %8

8:                                                ; preds = %4
  %indvars.iv.next = add nuw nsw i64 %indvars.iv7, 1
  %exitcond = icmp ne i64 %indvars.iv.next, 10
  br i1 %exitcond, label %4, label %9, !llvm.loop !15

9:                                                ; preds = %8
  br label %10

10:                                               ; preds = %16, %9
  %indvars.iv38 = phi i64 [ 0, %9 ], [ %indvars.iv.next4, %16 ]
  %11 = add nuw nsw i64 %indvars.iv38, 3
  %12 = getelementptr inbounds i32, ptr %3, i64 %11
  %13 = load i32, ptr %12, align 4
  %14 = add nsw i32 %13, 1
  %15 = getelementptr inbounds i32, ptr %3, i64 %indvars.iv38
  store i32 %14, ptr %15, align 4
  br label %16

16:                                               ; preds = %10
  %indvars.iv.next4 = add nuw nsw i64 %indvars.iv38, 1
  %exitcond6 = icmp ne i64 %indvars.iv.next4, 10
  br i1 %exitcond6, label %10, label %17, !llvm.loop !16

17:                                               ; preds = %16
  call void @llvm.stackrestore.p0(ptr %1)
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
