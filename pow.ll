; ModuleID = 'math_util'
source_filename = "math_util"

define i32 @main(i32 %base, i32 %exponent) {
entry:
  %res = alloca i32, align 4
  %exponent2 = alloca i32, align 4
  %base1 = alloca i32, align 4
  store i32 %base, ptr %base1, align 4
  store i32 %exponent, ptr %exponent2, align 4
  store i32 3, ptr %base1, align 4
  store i32 5, ptr %exponent2, align 4
  store i32 1, ptr %res, align 4
  br label %cond

cond:                                             ; preds = %body, %entry
  %0 = load i32, ptr %exponent2, align 4
  %1 = icmp sgt i32 %0, 0
  br i1 %1, label %body, label %merge

body:                                             ; preds = %cond
  %2 = load i32, ptr %res, align 4
  %3 = load i32, ptr %base1, align 4
  %4 = mul i32 %2, %3
  store i32 %4, ptr %res, align 4
  %5 = load i32, ptr %exponent2, align 4
  %6 = sub i32 %5, 1
  store i32 %6, ptr %exponent2, align 4
  br label %cond

merge:                                            ; preds = %cond
  %7 = load i32, ptr %res, align 4
  ret i32 %7
}
