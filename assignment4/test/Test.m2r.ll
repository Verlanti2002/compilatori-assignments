; ModuleID = 'test/Test.ll'
source_filename = "test/Test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@N = dso_local constant i32 10, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local void @loop_fusion1(ptr noundef %0, ptr noundef %1) #0 {
  br label %3

3:                                                ; preds = %2, %6
  %indvars.iv7 = phi i64 [ 0, %2 ], [ %indvars.iv.next, %6 ]
  %indvars.iv.next = add nuw nsw i64 %indvars.iv7, 1
  %4 = getelementptr inbounds i32, ptr %0, i64 %indvars.iv7
  %5 = trunc nuw nsw i64 %indvars.iv.next to i32
  store i32 %5, ptr %4, align 4
  br label %6

6:                                                ; preds = %3
  %exitcond = icmp ne i64 %indvars.iv.next, 10
  br i1 %exitcond, label %3, label %7, !llvm.loop !6

7:                                                ; preds = %6
  br label %8

8:                                                ; preds = %7, %12
  %indvars.iv38 = phi i64 [ 0, %7 ], [ %indvars.iv.next4, %12 ]
  %9 = getelementptr inbounds i32, ptr %0, i64 %indvars.iv38
  %10 = load i32, ptr %9, align 4
  %11 = getelementptr inbounds i32, ptr %1, i64 %indvars.iv38
  store i32 %10, ptr %11, align 4
  br label %12

12:                                               ; preds = %8
  %indvars.iv.next4 = add nuw nsw i64 %indvars.iv38, 1
  %exitcond6 = icmp ne i64 %indvars.iv.next4, 10
  br i1 %exitcond6, label %8, label %13, !llvm.loop !8

13:                                               ; preds = %12
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @loop_fusion2(ptr noundef %0, ptr noundef %1) #0 {
  br label %3

3:                                                ; preds = %2, %6
  %indvars.iv7 = phi i64 [ 0, %2 ], [ %indvars.iv.next, %6 ]
  %indvars.iv.next = add nuw nsw i64 %indvars.iv7, 1
  %4 = getelementptr inbounds i32, ptr %0, i64 %indvars.iv7
  %5 = trunc nuw nsw i64 %indvars.iv.next to i32
  store i32 %5, ptr %4, align 4
  br label %6

6:                                                ; preds = %3
  %exitcond = icmp ne i64 %indvars.iv.next, 10
  br i1 %exitcond, label %3, label %7, !llvm.loop !9

7:                                                ; preds = %6
  br label %8

8:                                                ; preds = %7, %15
  %indvars.iv38 = phi i64 [ 0, %7 ], [ %indvars.iv.next4, %15 ]
  %9 = add nuw nsw i64 %indvars.iv38, 3
  %10 = getelementptr inbounds i32, ptr %0, i64 %9
  %11 = load i32, ptr %10, align 4
  %12 = trunc nuw nsw i64 %indvars.iv38 to i32
  %13 = add nsw i32 %11, %12
  %14 = getelementptr inbounds i32, ptr %1, i64 %indvars.iv38
  store i32 %13, ptr %14, align 4
  br label %15

15:                                               ; preds = %8
  %indvars.iv.next4 = add nuw nsw i64 %indvars.iv38, 1
  %exitcond6 = icmp ne i64 %indvars.iv.next4, 10
  br i1 %exitcond6, label %8, label %16, !llvm.loop !10

16:                                               ; preds = %15
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @loop_fusion3(ptr noundef %0, ptr noundef %1, ptr noundef %2, ptr noundef %3) #0 {
  br label %5

5:                                                ; preds = %4, %12
  %indvars.iv12 = phi i64 [ 0, %4 ], [ %indvars.iv.next, %12 ]
  %6 = getelementptr inbounds i32, ptr %1, i64 %indvars.iv12
  %7 = load i32, ptr %6, align 4
  %8 = getelementptr inbounds i32, ptr %2, i64 %indvars.iv12
  %9 = load i32, ptr %8, align 4
  %10 = mul nsw i32 %7, %9
  %11 = getelementptr inbounds i32, ptr %0, i64 %indvars.iv12
  store i32 %10, ptr %11, align 4
  br label %12

12:                                               ; preds = %5
  %indvars.iv.next = add nuw nsw i64 %indvars.iv12, 1
  %exitcond = icmp ne i64 %indvars.iv.next, 10
  br i1 %exitcond, label %5, label %13, !llvm.loop !11

13:                                               ; preds = %12
  br label %14

14:                                               ; preds = %13, %21
  %indvars.iv413 = phi i64 [ 0, %13 ], [ %indvars.iv.next5, %21 ]
  %15 = getelementptr inbounds i32, ptr %0, i64 %indvars.iv413
  %16 = load i32, ptr %15, align 4
  %17 = getelementptr inbounds i32, ptr %2, i64 %indvars.iv413
  %18 = load i32, ptr %17, align 4
  %19 = add nsw i32 %16, %18
  %20 = getelementptr inbounds i32, ptr %3, i64 %indvars.iv413
  store i32 %19, ptr %20, align 4
  br label %21

21:                                               ; preds = %14
  %indvars.iv.next5 = add nuw nsw i64 %indvars.iv413, 1
  %exitcond7 = icmp ne i64 %indvars.iv.next5, 10
  br i1 %exitcond7, label %14, label %22, !llvm.loop !12

22:                                               ; preds = %21
  br label %23

23:                                               ; preds = %22, %28
  %indvars.iv814 = phi i64 [ 0, %22 ], [ %indvars.iv.next9, %28 ]
  %24 = getelementptr inbounds i32, ptr %2, i64 %indvars.iv814
  %25 = load i32, ptr %24, align 4
  %26 = sub nsw i32 %25, 1
  %27 = getelementptr inbounds i32, ptr %0, i64 %indvars.iv814
  store i32 %26, ptr %27, align 4
  br label %28

28:                                               ; preds = %23
  %indvars.iv.next9 = add nuw nsw i64 %indvars.iv814, 1
  %exitcond11 = icmp ne i64 %indvars.iv.next9, 10
  br i1 %exitcond11, label %23, label %29, !llvm.loop !13

29:                                               ; preds = %28
  ret void
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
!8 = distinct !{!8, !7}
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
!11 = distinct !{!11, !7}
!12 = distinct !{!12, !7}
!13 = distinct !{!13, !7}
