// bismillahirrahmanirrahim, bisa


#include <ArduTFLite.h>
#include "model.h"  // replace with the actual model header name

// Adjust Tensor Arena size as needed based on your model
constexpr int kTensorArenaSize = 8000;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

#define MAX_ROWS 1025
#define NUM_COLS_IN 2
#define NUM_COLS_OUT 1

float inputData[MAX_ROWS][NUM_COLS_IN];
float outputData[MAX_ROWS][NUM_COLS_OUT];

int rowIndex = 0;
String inputLine = "";

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 is ready to receive float arrays...");

  if (!modelInit(model, tensor_arena, kTensorArenaSize)) {
    Serial.println("Model failed to initialize!");
    while (true);
  }
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      inputLine.trim();
      if (inputLine.length() > 0) {
        if (inputLine.equalsIgnoreCase("RUN")) {
          tinyMLinference();
          showOutput();
        } 
        else {
          parseAndStoreRow(inputLine);
        }
      }
      inputLine = "";
    } 
    else {
      inputLine += c;
    }
  }
}

// Parses a CSV row in the format "float1,float2" and stores it in inputData
void parseAndStoreRow(String line) {

  if (rowIndex >= MAX_ROWS) {
    Serial.println("Array is full.");
    return;
  }

  int firstComma = line.indexOf(',');
  // int secondComma = line.indexOf(',', firstComma + 1);

  if (firstComma == -1) {
    Serial.println("Invalid input format. Expected: float1,float2");
    return;
  }

  float val1 = line.substring(0, firstComma).toFloat();
  float val2 = line.substring(firstComma + 1).toFloat();
  // float val3 = line.substring(secondComma + 1).toFloat();

  inputData[rowIndex][0] = val1;
  inputData[rowIndex][1] = val2;
  // inputData[rowIndex][2] = val3;

  Serial.printf("Stored row %d: %.2f, %.2f\n", rowIndex, val1, val2);
  rowIndex++;
}

// Runs TinyML inference on each stored row and stores outputs in output arrays
void tinyMLinference() {
  // int rowIndex = 0;

  if (rowIndex == 0) {
    Serial.println("No data to classify.");
    return;
  }
  
  Serial.println("Running inference on stored data...");
  // int countA = 0;
  // int countB = 0;
  
  // Process each row with the model.
  for (int i = 0; i < rowIndex; i++) {
    float output[2];  // Assuming the model outputs 2 scores for binary classification
    // modelRun() is provided by ArduTFLite; it copies inputData[i] into the model,
    // runs inference, and populates the output array.

    modelSetInput(inputData[i][0], 0);  // First feature
    modelSetInput(inputData[i][1], 1);  // Second feature
    // modelSetInput(inputData[i][2], 2);  // third feature

    if (!modelRunInference()) {
      Serial.printf("Inference failed at row %d\n", i);
      return;
    }

    float result = modelGetOutput(0);
    int classification = result >= 0.5 ? 1 : 0;  // Assuming threshold 0.5

    outputData[i][0] = classification;
  }

}

void showOutput() {

  int count0 = 0;
  int count1 = 0;

  Serial.println("Input and Output each row :");
  for (int i = 0; i < rowIndex; i++) {
    Serial.printf("[%d] %.2f, %.2f, %.2f \n", i, inputData[i][0], inputData[i][1], outputData[i][0]);
  
  // Counts which is most frequents
    if (outputData[i][0] == 0){
      count0++;
    } 
    else if (outputData[i][0] == 1){
      count1++;
    }
  }

  Serial.println("The final result is ");
  // Compare and display the most frequent value
  if (count0 > count1) {
    Serial.println("0 appears more frequently.");
  } 
  else if (count1 > count0) {
    Serial.println("1 appears more frequently.");
  }

  Serial.printf("Class 0 count: %d\n", count0);
  Serial.printf("Class 1 count: %d\n", count1);
  Serial.print("Most frequent class: ");
  Serial.println((count0 > count1) ? "0" : "1");

}
