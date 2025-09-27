# Latest Benchmarks (strict)

**task       impl          ns**
sort       tenge(qsort)  TASK=sort_qsort   N=100000   TIME_NS=9628208
sort       tenge(msort)  TASK=sort_msort   N=100000   TIME_NS=8614417
sort       tenge(pdq)    TASK=sort_pdq     N=100000   TIME_NS=5405750
sort       tenge(radix)  TASK=sort_radix   N=100000   TIME_NS=1182166
sort       c             TASK=sort         N=100000   TIME_NS=9576250    OK=1                        LAST=9223201037315965511
sort       rust          TASK=sort         N=100000   TIME_NS=2448542    ACC=37003713760312615
sort       go            TASK=sort         N=100000   TIME_NS=8265000
fib_iter   tenge         TASK=fib_iter     N=90       TIME_NS=41
fib_iter   c             38
fib_iter   rust          TASK=fib_iter     N=90       TIME_NS=40         SINK=4904717825630877312
fib_iter   go            35
fib_rec    tenge         TASK=fib_rec      N=35       TIME_NS=30300625
fib_rec    c             TASK=fib_rec      N=35       TIME_NS=31684167   RESULT=9227465
fib_rec    rust          TASK=fib_rec_rs   N=35       TIME_NS=30313208
fib_rec    go            TASK=fib_rec      N=35       TIME_NS=36360542   ACC=9227465
var_mc     tenge(sort)   TASK=var_mc_sort  N=1000000  TIME_NS=130378042
var_mc     tenge(zig)    TASK=var_mc_zig   N=1000000  TIME_NS=24649708
var_mc     tenge(qsel)   TASK=var_mc_qsel  N=1000000  TIME_NS=24057583
var_mc     c             TASK=var_mc       N=1000000  TIME_NS=148888000  VAR=2.876475
var_mc     rust          TASK=var_mc       N=1000000  TIME_NS=67268958   VAR=2.876475
var_mc     go            TASK=var_mc       N=1000000  TIME_NS=141731875  ACC=0.046570
nbody      tenge         TASK=nbody        N=4096     TIME_NS=253025542
nbody      c             TASK=nbody        N=4096     STEPS=10           DT=0.001000                 TIME_NS=389477000
nbody      rust          TASK=nbody        N=4096     TIME_NS=434672791  ENERGY=793771038.689191222
nbody      go            TASK=nbody        N=4096     TIME_NS=827425625  ENERGY=793771038.707027912
nbody_sym  tenge         TASK=nbody_sym    N=4096     TIME_NS=184755792
nbody_sym  c             TASK=nbody_sym    N=4096     STEPS=10           DT=0.001000                 TIME_NS=461135916
nbody_sym  rust          TASK=nbody_sym    N=4096     TIME_NS=450750458  ENERGY=957987877.410817981
nbody_sym  go            TASK=nbody_sym    N=4096     TIME_NS=436159000  ENERGY=957987877.417367458
