; ModuleID = 'module'
source_filename = "module"

define i64 @main() {
entry:
  %foo = alloca i64, align 8
  store i64 3, ptr %foo, align 4
  %foo1 = load i64, ptr %foo, align 4
  %addtmp = add i64 %foo1, 10
  ret i64 %addtmp
}
