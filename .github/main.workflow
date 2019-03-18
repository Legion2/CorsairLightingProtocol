workflow "Test" {
  on = "push"
  resolves = ["Build SimpleLightingController", "Build HoodLoader2LEDController"]
}

action "Install FastLED" {
  uses = "Legion2/download-release-action@master"
  args = "FastLED/FastLED 3.2.0"
  env = {
    TARGET_PATH = "libraries"
  }
}

action "Build SimpleLightingController" {
  uses = "Legion2/arduino-builder-action@master"
  needs = ["Install FastLED"]
  env = {
    BOARD_NAME = "arduino:avr:leonardo"
    SKETCH_PATH = "./examples/SimpleLightingController/SimpleLightingController.ino"
  }
}

action "Build HoodLoader2LEDController" {
  uses = "Legion2/arduino-builder-action@master"
  needs = ["Install FastLED"]
  env = {
    BOARD_NAME = "arduino:avr:mega"
    SKETCH_PATH = "./examples/HoodLoader2LEDController/HoodLoader2LEDController.ino"
  }
}
