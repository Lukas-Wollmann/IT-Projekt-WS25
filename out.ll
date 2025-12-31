; declare printf
declare i32 @printf(i8*, ...)

; format string global
@fmt = constant [4 x i8] c"%d\0A\00"

define i32 @main() {
entry:
  %tmp = alloca i32, align 4
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %res = alloca i32, align 4
  %n = alloca i32, align 4
  store i32 5, ptr %n, align 4
  store i32 1, ptr %res, align 4
  store i32 1, ptr %i, align 4
  br label %cond

cond:                                             ; preds = %after3, %entry
  %0 = load i32, ptr %i, align 4
  %1 = load i32, ptr %n, align 4
  %2 = icmp sle i32 %0, %1
  br i1 %2, label %body, label %after

body:                                             ; preds = %cond
  store i32 1, ptr %j, align 4
  store i32 0, ptr %tmp, align 4
  br label %cond1

after:                                            ; preds = %cond
  %3 = load i32, ptr %res, align 4
  ret i32 %3

cond1:                                            ; preds = %body2, %body
  %4 = load i32, ptr %j, align 4
  %5 = load i32, ptr %i, align 4
  %6 = icmp sle i32 %4, %5
  br i1 %6, label %body2, label %after3

body2:                                            ; preds = %cond1
  %7 = load i32, ptr %tmp, align 4
  %8 = load i32, ptr %j, align 4
  %9 = add i32 %7, %8
  store i32 %9, ptr %tmp, align 4
  %10 = load i32, ptr %j, align 4
  %11 = add i32 %10, 1
  store i32 %11, ptr %j, align 4
  br label %cond1

after3:                                           ; preds = %cond1
  %12 = load i32, ptr %res, align 4
  %13 = load i32, ptr %tmp, align 4
  %14 = mul i32 %12, %13
  store i32 %14, ptr %res, align 4

  ; ---- PRINTF HERE ----
  %15 = load i32, ptr %res, align 4
  %16 = getelementptr [4 x i8], [4 x i8]* @fmt, i32 0, i32 0
  call i32 (i8*, ...) @printf(i8* %16, i32 %15)
  ; --------------------

  %18 = load i32, ptr %i, align 4
  %19 = add i32 %18, 1
  store i32 %19, ptr %i, align 4
  br label %cond
}
