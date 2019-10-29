#include <timer.h>

auto timer = timer_create_default(); // create a timer with default settings

const byte pinLED = LED_BUILTIN;

void setup()
{
    pinMode( pinLED, OUTPUT );
    timer.every( 250, toggle_led, (const byte *)&pinLED );
   
}//setup

void loop()
{
    timer.tick();

}//loop

bool toggle_led( const byte *pin )
{       
    static bool
        state = false;

    digitalWrite( *pin, (state==true)?HIGH:LOW );
    state ^= true;

    return true;
   
}//toggle_led
