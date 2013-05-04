/* Public interface for MCP4802 DAC driver.

Martin Åberg, May 2013
*/

/* MCP4802/4812/4822 specific definitions */
#define DAC_ABn     _BV((15 - 8))
#define DAC_GAn     _BV((13 - 8))
#define DAC_SHDNn   _BV((12 - 8))
/* One of these constants shall be sent as parameter to the
dac_write function. */
#define DAC_A       ( (uint8_t)           DAC_GAn | DAC_SHDNn)
#define DAC_B       ( (uint8_t) DAC_ABn | DAC_GAn | DAC_SHDNn)

/* Set up pins connecting CPU and DAC. */
void dac_init(void);

/* Write 8 bits to the DAC. Select channel by setting config to
DAC_A or DAC_B. */
void dac_write(uint8_t data, uint8_t config);

