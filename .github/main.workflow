workflow "Issue Checker" {
  resolves = ["Issue Checklist Checker"]
  on = "issues"
}

action "Issue Checklist Checker" {
  uses = "adamzolyak/checklist-checker-action@1.1.1"
  secrets = ["GITHUB_TOKEN"]
}

workflow "Test" {
  on = "push"
  resolves = [
    "Build SimpleLightingController",
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

action "Build SimpleLightingController" {
  uses = "Legion2/arduino-builder-action@master"
  needs = ["Install FastLED"]
  env = {
    BOARD_NAME = "arduino:avr:leonardo"
    SKETCH_PATH = "./examples/SimpleLightingController/SimpleLightingController.ino"
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
