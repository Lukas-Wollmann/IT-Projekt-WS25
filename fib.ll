; ModuleID = 'module'
source_filename = "module"

define i32 @main() {
entry:
  %n = alloca i32, align 4
  store i32 20, ptr %n, align 4
  %a = alloca i32, align 4
  store i32 0, ptr %a, align 4
  %b = alloca i32, align 4
  store i32 1, ptr %b, align 4
  %temp = alloca i32, align 4
  store i32 0, ptr %temp, align 4
  %i = alloca i32, align 4
  store i32 2, ptr %i, align 4
  br label %cond

cond:                                             ; preds = %body, %entry
  %i1 = load i32, ptr %i, align 4
  %n2 = load i32, ptr %n, align 4
  %0 = icmp sle i32 %i1, %n2
  br i1 %0, label %body, label %merge

body:                                             ; preds = %cond
  %b3 = load i32, ptr %b, align 4
  store i32 %b3, ptr %temp, align 4
  %a4 = load i32, ptr %a, align 4
  %b5 = load i32, ptr %b, align 4
  %1 = add i32 %a4, %b5
  store i32 %1, ptr %b, align 4
  %temp6 = load i32, ptr %temp, align 4
  store i32 %temp6, ptr %a, align 4
  %i7 = load i32, ptr %i, align 4
  %2 = add i32 %i7, 1
  store i32 %2, ptr %i, align 4
  br label %cond

merge:                                            ; preds = %cond
  %b8 = load i32, ptr %b, align 4
  ret i32 %b8
}
