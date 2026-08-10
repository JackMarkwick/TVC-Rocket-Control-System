#include <SD.h>
#include <SPI.h>

class Logger {
  public:
    File dataFile;
    bool sdReady = false; //SPI mount confirmation register variable

    bool begin(int csPin) {
      if (SD.begin(csPin)) {
        sdReady = true;

        //Initialize structure for file
        dataFile = SD.open("Free4.csv", FILE_WRITE);
        if (dataFile) {
          //Indexing labels
          dataFile.println("Time_ms, Pitch, Roll, Yaw, outL1, outR1, outL2, outR2");
          dataFile.close(); //Sector clear down
          return true;
        }
      }
      return false;
    }

    void logData(unsigned long ms, float p, float r, float y, float sL1, float sR1, float sL2, float sR2) {
      if (!sdReady) return;

      //Open storage file on local cache regions
      dataFile = SD.open("Free4.csv", FILE_WRITE);
      if (dataFile) {
        dataFile.print(ms);
        dataFile.print(",");
        dataFile.print(p);
        dataFile.print(",");
        dataFile.print(r);
        dataFile.print(",");
        dataFile.print(y);
        dataFile.print(",");
        dataFile.print(sL1);
        dataFile.print(",");
        dataFile.print(sR1);
        dataFile.print(",");
        dataFile.print(sL2);
        dataFile.print(",");
        dataFile.println(sR2);
        dataFile.close();
      }
    }
};