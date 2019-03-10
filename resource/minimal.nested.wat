(module
 (export "squarer" (func $squarer))
 (func $squarer (; 0 ;) (param $0 i32) (result i32)
  (i32.mul
   (get_local $0)
   (get_local $0)
  )
 )
)
