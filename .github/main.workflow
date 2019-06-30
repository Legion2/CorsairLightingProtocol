workflow "Test" {
  on = "push"
  resolves = [
    "Build LightingNodePRO",
    "Build CommanderPRO",
    "Build SingleStripLightingNodePRO",
    "Build HoodLoader2UnoMegaController",
  ]
}

action "Install FastLED" {
  uses = "Legion2/download-release-action@master"
  args = "FastLED/FastLED 3.2.0"
  env = {
    TARGET_PATH = "libraries"
  }
}

action "Build LightingNodePRO" {
  uses = "Legion2/arduino-builder-action@master"
  needs = ["Install FastLED"]
  env = {
    BOARD_NAME = "arduino:avr:leonardo"
    SKETCH_PATH = "./examples/LightingNodePRO/LightingNodePRO.ino"
  }
}

action "Build CommanderPRO" {
  uses = "Legion2/arduino-builder-action@master"
  needs = ["Install FastLED"]
  env = {
    BOARD_NAME = "arduino:avr:leonardo"
    SKETCH_PATH = "./examples/CommanderPRO/CommanderPRO.ino"
  }
}

action "Build SingleStripLightingNodePRO" {
  uses = "Legion2/arduino-builder-action@master"
  needs = ["Install FastLED"]
  env = {
    BOARD_NAME = "arduino:avr:micro"
    SKETCH_PATH = "./examples/LightingNodePRO/SingleStripLightingNodePRO.ino"
  }
}

action "Build HoodLoader2UnoMegaController" {
  uses = "Legion2/arduino-builder-action@master"
  needs = ["Install FastLED"]
  env = {
    BOARD_NAME = "arduino:avr:uno"
    SKETCH_PATH = "./examples/HoodLoader2UnoMegaController/HoodLoader2UnoMegaController.ino"
  }
}
