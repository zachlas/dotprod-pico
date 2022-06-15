#include <stdio.h>
#include "arm_math.h"
#include "pico/stdlib.h"
#include "systick.h"

#define force_inline always_inline

#define float32_t float
#define MAX_BLOCKSIZE    32 
#define DELTA           (0.000001f)

/* ----------------------------------------------------------------------
** Test input data of srcA for blockSize 32
** ------------------------------------------------------------------- */
float32_t srcA[MAX_BLOCKSIZE] =
{
  -0.4325648115282207,  -1.6655843782380970,  0.1253323064748307,
   0.2876764203585489,  -1.1464713506814637,  1.1909154656429988,
   1.1891642016521031,  -0.0376332765933176,  0.3272923614086541,
   0.1746391428209245,  -0.1867085776814394,  0.7257905482933027, 
  -0.5883165430141887,   2.1831858181971011, -0.1363958830865957,
   0.1139313135208096,   1.0667682113591888,  0.0592814605236053,
  -0.0956484054836690,  -0.8323494636500225,  0.2944108163926404,
  -1.3361818579378040,   0.7143245518189522,  1.6235620644462707,
  -0.6917757017022868,   0.8579966728282626,  1.2540014216025324,
  -1.5937295764474768,  -1.4409644319010200,  0.5711476236581780,
  -0.3998855777153632,   0.6899973754643451
};
/* ----------------------------------------------------------------------
** Test input data of srcB for blockSize 32
** ------------------------------------------------------------------- */
float32_t srcB[MAX_BLOCKSIZE] =
{
   1.7491401329284098,  0.1325982188803279,   0.3252281811989881,
  -0.7938091410349637,  0.3149236145048914,  -0.5272704888029532,
   0.9322666565031119,  1.1646643544607362,  -2.0456694357357357,
  -0.6443728590041911,  1.7410657940825480,   0.4867684246821860,
   1.0488288293660140,  1.4885752747099299,   1.2705014969484090,
  -1.8561241921210170,  2.1343209047321410,   1.4358467535865909,
  -0.9173023332875400, -1.1060770780029008,   0.8105708062681296,
   0.6985430696369063, -0.4015827425012831,   1.2687512030669628,
  -0.7836083053674872,  0.2132664971465569,   0.7878984786088954,
   0.8966819356782295, -0.1869172943544062,   1.0131816724341454,
   0.2484350696132857,  0.0596083377937976
};


/* Reference dot product output */
float32_t  refDotProdOut = 5.9273644806352142;

float32_t multOutput[MAX_BLOCKSIZE];  /* Intermediate output */
float32_t testOutput;  /* Final ouput */
arm_status status;   /* Status of the example */



float dot_product(float v1[], float v2[], int length);
float dot_product2(float v1[], float v2[], int length);
float dot_product3(float v1[], float v2[], int length);

float dot_product3(float v1[], float v2[], int length)
{

    float result;

    (void) start_systick();

    (void) arm_dot_prod_f32(v1, v2, length, &result);

    (void) stop_systick();

    return result;

}

float dot_product2(float v1[], float v2[], int length)
{
    uint32_t i;         /* Loop counter */
    float32_t diff;     /* Difference between reference and test outputs */

    (void) start_systick();

    /* Multiplication of two input buffers */
    arm_mult_f32(v1, v2, multOutput, MAX_BLOCKSIZE);

    /* Accumulate the multiplication output values to
       get the dot product of the two inputs */
    for(i=0; i< MAX_BLOCKSIZE; i++)
    {
        arm_add_f32(&testOutput, &multOutput[i], &testOutput, 1);
    }

    (void) stop_systick();

    /* absolute value of difference between ref and test */
    diff = fabsf(refDotProdOut - testOutput);

    /* Comparison of dot product value with reference */
    status = (diff > DELTA) ? ARM_MATH_TEST_FAILURE : ARM_MATH_SUCCESS;
  
  if (status != ARM_MATH_SUCCESS)
  {
    printf("FAILURE\n");
  }
  else
  {
    printf("SUCCESS\n");
  }
  return testOutput ;
}


// accepts two vectors (arrays) as arguments and a length, computes and returns
// the dot product
float dot_product(float v1[], float v2[], int length)
{
    // we compute the dot product by multiply the correspdoning elements in each 
    // array and adding them to a sum
    float sum = 0;

    (void) start_systick();

    for (int i = 0; i < length; i++)
      sum += v1[i] * v2[i];

    (void) stop_systick();

    return sum;
}

int main() 
{
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    stdio_init_all();

    float32_t result = dot_product(srcA, srcB, MAX_BLOCKSIZE);
    printf("Dot product result: %f\n", result);

    result = dot_product2(srcA, srcB, MAX_BLOCKSIZE);
    printf("Dot product2 result: %f\n", result);

    result = dot_product3(srcA, srcB, MAX_BLOCKSIZE);
    printf("Dot product3 result: %f\n", result);

    while (true) 
    {
//        printf("Hello, Pi Pico!\n");
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }

    return 0;
}

