// Bakeoff #3 - Escrita em Smartwatches
// IPM 2020-21, Semestre 2
// Entrega: até dia 4 de Junho às 23h59 através do Fenix
// Bake-off: durante os laboratórios da semana de 31 de Maio

// p5.js reference: https://p5js.org/reference/

// Database (CHANGE THESE!)
const GROUP_NUMBER = 54; // add your group number here as an integer (e.g., 2, 3)
const BAKE_OFF_DAY = true; // set to 'true' before sharing during the simulation and bake-off days

let PPI, PPCM; // pixel density (DO NOT CHANGE!)
let second_attempt_button; // button that starts the second attempt (DO NOT CHANGE!)

// Finger parameters (DO NOT CHANGE!)
let finger_img; // holds our finger image that simules the 'fat finger' problem
let FINGER_SIZE, FINGER_OFFSET; // finger size and cursor offsett (calculated after entering fullscreen)

// Arm parameters (DO NOT CHANGE!)
let arm_img; // holds our arm/watch image
let ARM_LENGTH, ARM_HEIGHT; // arm size and position (calculated after entering fullscreen)

// Study control parameters (DO NOT CHANGE!)
let draw_finger_arm = false; // used to control what to show in draw()
let phrases = []; // contains all 501 phrases that can be asked of the user
let current_trial = 0; // the current trial out of 2 phrases (indexes into phrases array above)
let attempt = 0; // the current attempt out of 2 (to account for practice)
let target_phrase = ""; // the current target phrase
let currently_typed = ""; // what the user has typed so far
let entered = new Array(2); // array to store the result of the two trials (i.e., the two phrases entered in one attempt)
let CPS = 0; // add the characters per second (CPS) here (once for every attempt)

// Metrics
let attempt_start_time, attempt_end_time; // attemps start and end times (includes both trials)
let trial_end_time; // the timestamp of when the lastest trial was completed
let letters_entered = 0; // running number of letters entered (for final WPM computation)
let letters_expected = 0; // running number of letters expected (from target phrase)
let errors = 0; // a running total of the number of errors (when hitting 'ACCEPT')
let database; // Firebase DB

// 2D Keyboard UI

let displayedSection; // 0 = show default keyboard | [1, ..., 6] = show zoomed in sections
let keyboard; // to store default full keyboard image
let zxautoSection; // to store z, x and auto complete key section of keyboard image
let cvbspaceSection; // to store c, v, b and space key section of keyboard image
let nmbackSection; // to store n, m and backspace key section of keyboard image
let qweasdSection; // to store q, w, e, a, s and d key section of keyboard image
let rtyufghSection; // to store r, t, y, u, f, g and h key section of keyboard image
let iopjklSection; // to store i, o, p, j, k and l key section of keyboard image

let firstRowSpacing; // width of first row keys
let secondRowSpacing; // width of second row keys
let thirdRowSpacing; // width of third row keys
let fourthRowSpacing; // width of fourth row keys
let verticalSpacing; // height of each key
let ARROW_SIZE; // UI button size
let current_letter = "a"; // current char being displayed on our basic 2D keyboard (starts with 'a')
let suggestions = [];
let current_suggestion = " ";

let first_suggestion = "";
let second_suggestion = "";
let autocomplete1 = "";
let autocomplete2 = "";

// Runs once before the setup() and loads our data (images, phrases)
function preload() {
  // Loads simulation images (arm, finger) -- DO NOT CHANGE!
  arm = loadImage("data/arm_watch.png");
  fingerOcclusion = loadImage("data/finger.png");

  // Loads the target phrases (DO NOT CHANGE!)
  phrases = loadStrings("data/phrases.txt");

  // Loads UI elements for our basic keyboard
  keyboard = loadImage("HQKB_3A/HQ_grayred_big.png");
  zxautoSection = loadImage("HQKB_3A/zx_graywhite.png");
  cvbspaceSection = loadImage("HQKB_3A/cvbspace_graywhite.png");
  nmbackSection = loadImage("HQKB_3A/nmbackspace_graywhite.png");
  qweasdSection = loadImage("HQKB_3A/qweasd_graywhite.png");
  rtyufghSection = loadImage("HQKB_3A/rtyufgh_graywhite.png");
  iopjklSection = loadImage("HQKB_3A/iopjkl_graywhite.png");

  suggestions = loadStrings("data/count.txt");
}

// Runs once at the start
function setup() {
  createCanvas(700, 500); // window size in px before we go into fullScreen()
  frameRate(60); // frame rate (DO NOT CHANGE!)

  // DO NOT CHANGE THESE!
  shuffle(phrases, true); // randomize the order of the phrases list (N=501)
  target_phrase = phrases[current_trial];

  drawUserIDScreen(); // draws the user input screen (student number and display size)

  firstRowSpacing = (4 * PPCM) / 10;
  secondRowSpacing = (4 * PPCM) / 9;
  thirdRowSpacing = (4 * PPCM) / 7;
  fourthRowSpacing = (4 * PPCM) / 3;
  verticalSpacing = (3 * PPCM) / 4;
  displayedArea = 0;
}

function draw() {
  if (draw_finger_arm) {
    background(255); // clear background
    noCursor(); // hides the cursor to simulate the 'fat finger'

    drawArmAndWatch(); // draws arm and watch background
    writeTargetAndEntered(); // writes the target and entered phrases above the watch
    drawACCEPT(); // draws the 'ACCEPT' button that submits a phrase and completes a trial

    // Draws the touch input area (4x3cm) -- DO NOT CHANGE SIZE!
    stroke(0, 255, 0);
    noFill();
    rect(
      width / 2 - 2.0 * PPCM,
      height / 2 - 1.0 * PPCM,
      4.0 * PPCM,
      3.0 * PPCM
    );

    draw2Dkeyboard(); // draws our basic 2D keyboard UI
    drawPredictive();
    drawFatFinger(); // draws the finger that simulates the 'fat finger' problem
  }
}

function getLastWord(string) {
  var n = string.split(" ");
  return n[n.length - 1];
}

function drawPredictive() {
  noStroke();
  let fsl = first_suggestion.length;
  let ssl = second_suggestion.length;
  
  if (fsl == ssl) {
    fill(118, 197, 227);
    rect(width / 2 - 2.0 * PPCM, height / 2 - 2.0 * PPCM, 2.0 * PPCM, 1.0 * PPCM);
    fill(250, 144, 144);
    rect(width / 2, height / 2 - 2.0 * PPCM, 2.0 * PPCM, 1.0 * PPCM);
    textAlign(CENTER);
    textFont("Arial", 17 - fsl / 3);
    fill(0);
    text(first_suggestion, width / 2 - 1.0 * PPCM, height / 2 - 1.3 * PPCM)
    text(second_suggestion, width / 2 + 1.0 * PPCM, height / 2 - 1.3 * PPCM)
  } else if (fsl != ssl) {
    
    fill(118, 197, 227);
    // rectangulo azul
    rect(width / 2 - 2.0 * PPCM,
         height / 2 - 2.0 * PPCM,
         fsl/(fsl+ssl) * 4.0 * PPCM,
         1.0 * PPCM);
    
    fill(250, 144, 144);
    // rectangulo rosa
    rect(width / 2 - 2.0 * PPCM + fsl/(fsl+ssl) * 4.0 * PPCM,
         height / 2 - 2.0 * PPCM,
         ssl/(fsl+ssl) * 4.0 * PPCM,
         1.0 * PPCM);
    
    textAlign(CENTER);
    textFont("Arial", 17 - fsl / 3);
    fill(0);
    text( first_suggestion,
          width / 2 - 2.0 * PPCM + fsl/(fsl+ssl) * 2.0 * PPCM,
          height / 2 - 1.3 * PPCM
        );
    textFont("Arial", 17 - ssl / 3);
    text( second_suggestion,
          width / 2 - 2.0 * PPCM + fsl/(fsl+ssl) * 4.0 * PPCM + ssl/(fsl+ssl) * 2.0 * PPCM,
          height / 2 - 1.3 * PPCM)
  }
  //textFont("Arial", 16 - fsl / 3);
  //fill(0);
  //text(first_suggestion, width / 2 - 1.0 * PPCM, height / 2 - 1.3 * PPCM);
  //textFont("Arial", 16 - ssl / 3);
  //text(second_suggestion, width / 2 + 1.0 * PPCM, height / 2 - 1.3 * PPCM);
}

function drawSuggestion() {
  let n = currently_typed.split(" ");
  let lastWord = n[n.length - 1];
  let index;
  let i = 0;
  if (lastWord.length != 0) {
    length1 = lastWord.length;
    for (index = 0; index < 333333; index++) {
      aux1 = suggestions[index].substring(0, length1);
      if (aux1 === lastWord) {
        first_suggestion = suggestions[index];
        length2 = first_suggestion.length;
        for (i = 0; i < length2; i++) {
          if (/\s/.test(first_suggestion.charAt(i))) {
            first_suggestion = first_suggestion.substring(0, i);
            break;
          }
        }
        autocomplete1 = first_suggestion.substring(length1, i) + " ";
        break;
      }
    }
    index++;
    for (index; index < 333333; index++) {
      aux1 = suggestions[index].substring(0, length1);
      if (aux1 === lastWord) {
        second_suggestion = suggestions[index];
        length2 = second_suggestion.length;
        for (i = 0; i < length2; i++) {
          if (/\s/.test(second_suggestion.charAt(i))) {
            second_suggestion = second_suggestion.substring(0, i);
            break;
          }
        }
        autocomplete2 = second_suggestion.substring(length1, i) + " ";
        break;
      }
    }
    current_suggestion = first_suggestion + "  " + second_suggestion;
  } else {
    current_suggestion = " ";
    autocomplete1 = "";
    autocomplete2 = "";
  }
}

// Draws 2D keyboard UI (current letter and left and right arrows)
function draw2Dkeyboard() {
  switch (displayedArea) {
    case 1: // zx + auto complete key section
      image(
        zxautoSection,
        width / 2,
        height / 2 + 0.5 * PPCM,
        4.0 * PPCM,
        3.0 * PPCM
      );
      break;
    case 2: // cvb + space key section
      image(
        cvbspaceSection,
        width / 2,
        height / 2 + 0.5 * PPCM,
        4.0 * PPCM,
        3.0 * PPCM
      );
      break;
    case 3: // nm + backspace key section
      image(
        nmbackSection,
        width / 2,
        height / 2 + 0.5 * PPCM,
        4.0 * PPCM,
        3.0 * PPCM
      );
      break;
    case 4: // qweasd key section
      image(
        qweasdSection,
        width / 2,
        height / 2 + 0.5 * PPCM,
        4.0 * PPCM,
        3.0 * PPCM
      );
      break;
    case 5: // rtyufgh key section
      image(
        rtyufghSection,
        width / 2,
        height / 2 + 0.5 * PPCM,
        4.0 * PPCM,
        3.0 * PPCM
      );
      break;
    case 6: // iopjkl key section
      image(
        iopjklSection,
        width / 2,
        height / 2 + 0.5 * PPCM,
        4.0 * PPCM,
        3.0 * PPCM
      );
      break;
    default:
      // default keyboard
      image(
        keyboard,
        width / 2,
        height / 2 + 0.5 * PPCM,
        4.0 * PPCM,
        3.0 * PPCM
      );
  }
}

// Evoked when the mouse button was pressed
function mousePressed() {
  // Only look for mouse presses during the actual test
  if (draw_finger_arm) {
    // Check if mouse click happened within the touch input area
    if (
      mouseClickWithin(
        width / 2 - 2.0 * PPCM,
        height / 2 - 1.0 * PPCM,
        4.0 * PPCM,
        3.0 * PPCM
      )
    ) {
      if (displayedArea == 0) {
        // if default keyboard is displayed, check where mouse was clicked
        // check for click on the zxauto section
        if (
          mouseClickWithin(
            width / 2 - 2.0 * PPCM, // start on 1st key
            height / 2 - 1.0 * PPCM + 2 * verticalSpacing, // start on 3rd row
            1.5 * thirdRowSpacing, // catch 3 keys  (zx / auto complete)
            2 * verticalSpacing // catch 2 rows
          )
        ) {
          displayedArea = 1;
        }
        // check for click on the cvbspace section
        else if (
          mouseClickWithin(
            width / 2 - 2.0 * PPCM + 1.5 * thirdRowSpacing, // start on 3rd key (c)
            height / 2 - 1.0 * PPCM + 2 * verticalSpacing, // start on 3rd row
            3 * thirdRowSpacing, // catch 3 keys (cvb / space)
            2 * verticalSpacing // catch 2 rows
          )
        ) {
          displayedArea = 2;
        }
        // check for click on the nmback section
        else if (
          mouseClickWithin(
            width / 2 - 2.0 * PPCM + 4.5 * thirdRowSpacing, // start on 6th key (n)
            height / 2 - 1.0 * PPCM + 2 * verticalSpacing, // start on 3rd row
            2 * thirdRowSpacing, // catch 2 keys (nm / backspace)
            2 * verticalSpacing // catch 2 rows
          )
        ) {
          displayedArea = 3;
        }
        // check for click on the qweasd section
        else if (
          mouseClickWithin(
            width / 2 - 2.0 * PPCM, // start on 1st key (q)
            height / 2 - 1.0 * PPCM, // start on 1st row
            3 * firstRowSpacing, // catch 3 keys (qwe)
            1 * verticalSpacing // catch 1 row
          ) ||
          mouseClickWithin(
            width / 2 - 2.0 * PPCM, // start on 1st key (a)
            height / 2 - 1.0 * PPCM + 1 * verticalSpacing, // start on 2nd row
            3 * secondRowSpacing, // catch 3 keys (asd)
            1 * verticalSpacing // catch 1 row
          )
        ) {
          displayedArea = 4;
        }
        // check for click on the rtyufgh section
        else if (
          mouseClickWithin(
            width / 2 - 2.0 * PPCM + 3 * firstRowSpacing, // start on 4th key (r)
            height / 2 - 1.0 * PPCM, // start on 1st row
            4 * firstRowSpacing, // catch 4 keys (rtyu)
            1 * verticalSpacing // catch 1 row
          ) ||
          mouseClickWithin(
            width / 2 - 2.0 * PPCM + 3 * secondRowSpacing, // start on 4th key (f)
            height / 2 - 1.0 * PPCM + 1 * verticalSpacing, // start on 2nd row
            3 * secondRowSpacing, // catch 3 keys (fgh)
            1 * verticalSpacing // catch 1 row
          )
        ) {
          displayedArea = 5;
        }
        // check for click on the iopjkl section
        else if (
          mouseClickWithin(
            width / 2 - 2.0 * PPCM + 7 * firstRowSpacing, // start on 8th key (i)
            height / 2 - 1.0 * PPCM, // start on 1st row
            3 * firstRowSpacing, // catch 3 keys (iop)
            1 * verticalSpacing // catch 1 row
          ) ||
          mouseClickWithin(
            width / 2 - 2.0 * PPCM + 6 * secondRowSpacing, // start on 7th key (j)
            height / 2 - 1.0 * PPCM + 1 * verticalSpacing, // start on 2nd row
            3 * secondRowSpacing, // catch 3 keys (jkl)
            1 * verticalSpacing // catch 1 row
          )
        ) {
          displayedArea = 6;
        }
      } else {
        // if displayedArea != 0 the keyboard is 'zoomed in'
        // When keyboard is zoomed in, next click/touch is key selection
        // process clicked/touched key here
        switch (displayedArea) {
          case 1: // when zx + auto complete section is displayed
            if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM,
                4.0 * PPCM,
                1.0 * verticalSpacing
              )
            ) {
              break;
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM + 1.0 * verticalSpacing,
                5 * firstRowSpacing,
                1.75 * verticalSpacing
              )
            ) {
              currently_typed += "z";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 5 * firstRowSpacing,
                height / 2 - 1.0 * PPCM + 1.0 * verticalSpacing,
                5 * firstRowSpacing,
                1.75 * verticalSpacing
              )
            ) {
              currently_typed += "x";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM + 2.75 * verticalSpacing,
                5 * firstRowSpacing,
                2.25 * verticalSpacing
              )
            ) {
              currently_typed += autocomplete1;
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2,
                height / 2 - 1.0 * PPCM + 2.75 * verticalSpacing,
                5 * firstRowSpacing,
                2.25 * verticalSpacing
              )
            ) {
              currently_typed += autocomplete2;
              drawSuggestion();
            }
            break;
          case 2: // when cvb + space section is displayed
            if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM,
                4.0 * PPCM,
                1.0 * verticalSpacing
              )
            ) {
              break;
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM + 1.0 * verticalSpacing,
                3 * secondRowSpacing,
                1.75 * verticalSpacing
              )
            ) {
              // 'c' selected
              currently_typed += "c";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 3 * secondRowSpacing,
                height / 2 - 1.0 * PPCM + 1.0 * verticalSpacing,
                3 * secondRowSpacing,
                1.75 * verticalSpacing
              )
            ) {
              // 'v' selected
              currently_typed += "v";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 6 * secondRowSpacing,
                height / 2 - 1.0 * PPCM + 1.0 * verticalSpacing,
                3 * secondRowSpacing,
                1.75 * verticalSpacing
              )
            ) {
              // 'b' selected
              currently_typed += "b";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM + 2.75 * verticalSpacing,
                9 * secondRowSpacing,
                2.25 * verticalSpacing
              )
            ) {
              // space key selected
              currently_typed += " ";
              drawSuggestion();
            }
            break;
          case 3: // when nm + backspace section is displayed
            if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM,
                4.0 * PPCM,
                1.0 * verticalSpacing
              )
            ) {
              break;
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM + 1.0 * verticalSpacing,
                5 * firstRowSpacing,
                1.75 * verticalSpacing
              )
            ) {
              // 'n' key selected
              currently_typed += "n";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 5 * firstRowSpacing,
                height / 2 - 1.0 * PPCM + 1.0 * verticalSpacing,
                5 * firstRowSpacing,
                1.75 * verticalSpacing
              )
            ) {
              // 'm' key selected
              currently_typed += "m";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM + 2.75 * verticalSpacing,
                10 * firstRowSpacing,
                2.25 * verticalSpacing
              )
            ) {
              currently_typed = currently_typed.substr(
                0,
                currently_typed.length - 1
              );
              drawSuggestion();
            }
            break;
          case 4:
            if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 + 1.5 * PPCM,
                4.0 * PPCM,
                1.0 * verticalSpacing
              )
            ) {
              break;
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM,
                3 * firstRowSpacing,
                1.7 * verticalSpacing
              )
            ) {
              // 'q' selected
              currently_typed += "q";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 3 * firstRowSpacing,
                height / 2 - 1.0 * PPCM,
                3 * firstRowSpacing,
                1.7 * verticalSpacing
              )
            ) {
              // 'w' selected
              currently_typed += "w";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 6 * firstRowSpacing,
                height / 2 - 1.0 * PPCM,
                4 * firstRowSpacing,
                1.7 * verticalSpacing
              )
            ) {
              // 'e' selected
              currently_typed += "e";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM + 1.7 * verticalSpacing,
                3.5 * firstRowSpacing,
                2.3 * verticalSpacing
              )
            ) {
              // 'a' selected
              currently_typed += "a";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 3.5 * firstRowSpacing,
                height / 2 - 1.0 * PPCM + 1.7 * verticalSpacing,
                3 * firstRowSpacing,
                2.3 * verticalSpacing
              )
            ) {
              // 's' selected
              currently_typed += "s";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 7 * firstRowSpacing,
                height / 2 - 1.0 * PPCM + 1.7 * verticalSpacing,
                3 * firstRowSpacing,
                2.3 * verticalSpacing
              )
            ) {
              // 'd' selected
              currently_typed += "d";
              drawSuggestion();
            }
            break;
          case 5:
            if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 + 1.5 * PPCM,
                4.0 * PPCM,
                1.0 * verticalSpacing
              )
            ) {
              break;
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM,
                2.5 * firstRowSpacing,
                1.7 * verticalSpacing
              )
            ) {
              // 'r' selected
              currently_typed += "r";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 2.5 * firstRowSpacing,
                height / 2 - 1.0 * PPCM,
                2.5 * firstRowSpacing,
                1.7 * verticalSpacing
              )
            ) {
              // 't' selected
              currently_typed += "t";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 5 * firstRowSpacing,
                height / 2 - 1.0 * PPCM,
                2.5 * firstRowSpacing,
                1.7 * verticalSpacing
              )
            ) {
              // 'y' selected
              //print('y');
              currently_typed += "y";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 7.5 * firstRowSpacing,
                height / 2 - 1.0 * PPCM,
                2.5 * firstRowSpacing,
                1.7 * verticalSpacing
              )
            ) {
              // 'u' selected
              //print('u');
              currently_typed += "u";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM + 1.7 * verticalSpacing,
                3.25 * firstRowSpacing,
                2.3 * verticalSpacing
              )
            ) {
              // 'f' selected
              //print('f');
              currently_typed += "f";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 3.25 * firstRowSpacing,
                height / 2 - 1.0 * PPCM + 1.7 * verticalSpacing,
                2.75 * firstRowSpacing,
                2.3 * verticalSpacing
              )
            ) {
              // 'g' selected
              // print('g');
              currently_typed += "g";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 6 * firstRowSpacing,
                height / 2 - 1.0 * PPCM + 1.7 * verticalSpacing,
                3 * firstRowSpacing,
                2.3 * verticalSpacing
              )
            ) {
              // 'h' selected
              //print('h');
              currently_typed += "h";
              drawSuggestion();
            }
            break;
          case 6:
            if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 + 1.5 * PPCM,
                4.0 * PPCM,
                1.0 * verticalSpacing
              )
            ) {
              break;
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM,
                3.5 * secondRowSpacing,
                1.7 * verticalSpacing
              )
            ) {
              // 'i' selected
              //print('i');
              currently_typed += "i";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 3.5 * secondRowSpacing,
                height / 2 - 1.0 * PPCM,
                2.5 * secondRowSpacing,
                1.7 * verticalSpacing
              )
            ) {
              // 'o' selected
              //print('o');
              currently_typed += "o";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 6 * secondRowSpacing,
                height / 2 - 1.0 * PPCM,
                3 * secondRowSpacing,
                1.7 * verticalSpacing
              )
            ) {
              // 'p' selected
              //print('p');
              currently_typed += "p";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM,
                height / 2 - 1.0 * PPCM + 1.7 * verticalSpacing,
                2.5 * secondRowSpacing,
                2.3 * verticalSpacing
              )
            ) {
              // 'j' selected
              //print('j');
              currently_typed += "j";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 2.5 * secondRowSpacing,
                height / 2 - 1.0 * PPCM + 1.7 * verticalSpacing,
                2.5 * secondRowSpacing,
                2.3 * verticalSpacing
              )
            ) {
              // 'k' selected
              //print('k');
              currently_typed += "k";
              drawSuggestion();
            } else if (
              mouseClickWithin(
                width / 2 - 2.0 * PPCM + 5 * secondRowSpacing,
                height / 2 - 1.0 * PPCM + 1.7 * verticalSpacing,
                4 * secondRowSpacing,
                2.3 * verticalSpacing
              )
            ) {
              // 'l' selected
              //print('l');
              currently_typed += "l";
              drawSuggestion();
            }
            break;
        }
        displayedArea = 0;
      }
    }

    // Check if mouse click happened within 'ACCEPT'
    // (i.e., submits a phrase and completes a trial)
    else if (
      mouseClickWithin(
        width / 2 - 2 * PPCM,
        height / 2 - 5.1 * PPCM,
        4.0 * PPCM,
        2.0 * PPCM
      )
    ) {
      // Saves metrics for the current trial
      letters_expected += target_phrase.trim().length;
      letters_entered += currently_typed.trim().length;
      errors += computeLevenshteinDistance(
        currently_typed.trim(),
        target_phrase.trim()
      );
      entered[current_trial] = currently_typed;
      trial_end_time = millis();

      current_trial++;

      // Check if the user has one more trial/phrase to go
      if (current_trial < 2) {
        // Prepares for new trial
        currently_typed = "";
        target_phrase = phrases[current_trial];
      } else {
        // The user has completed both phrases for one attempt
        draw_finger_arm = false;
        attempt_end_time = millis();

        printAndSavePerformance(); // prints the user's results on-screen and sends these to the DB
        attempt++;

        // Check if the user is about to start their second attempt
        if (attempt < 2) {
          second_attempt_button = createButton("START 2ND ATTEMPT");
          second_attempt_button.mouseReleased(startSecondAttempt);
          second_attempt_button.position(
            width / 2 - second_attempt_button.size().width / 2,
            height / 2 + 200
          );
        }
      }
    }
  }
}

// Resets variables for second attempt
function startSecondAttempt() {
  // Re-randomize the trial order (DO NOT CHANG THESE!)
  shuffle(phrases, true);
  current_trial = 0;
  target_phrase = phrases[current_trial];

  // Resets performance variables (DO NOT CHANG THESE!)
  letters_expected = 0;
  letters_entered = 0;
  errors = 0;
  currently_typed = "";
  CPS = 0;

  current_letter = "a";

  // Show the watch and keyboard again
  second_attempt_button.remove();
  draw_finger_arm = true;
  attempt_start_time = millis();
}

// Print and save results at the end of 2 trials
function printAndSavePerformance() {
  // DO NOT CHANGE THESE
  let attempt_duration = (attempt_end_time - attempt_start_time) / 60000; // 60K is number of milliseconds in minute
  let wpm = letters_entered / 5.0 / attempt_duration;
  let freebie_errors = letters_expected * 0.05; // no penalty if errors are under 5% of chars
  let penalty = max(0, (errors - freebie_errors) / attempt_duration);
  let wpm_w_penalty = max(wpm - penalty, 0); // minus because higher WPM is better: NET WPM
  let timestamp =
    day() +
    "/" +
    month() +
    "/" +
    year() +
    "  " +
    hour() +
    ":" +
    minute() +
    ":" +
    second();
  
  CPS = letters_entered / ((attempt_end_time - attempt_start_time)/1000);
  background(color(0, 0, 0)); // clears screen
  cursor(); // shows the cursor again

  textFont("Arial", 16); // sets the font to Arial size 16
  fill(color(255, 255, 255)); //set text fill color to white
  text(timestamp, 100, 20); // display time on screen

  text(
    "Finished attempt " + (attempt + 1) + " out of 2!",
    width / 2,
    height / 2
  );

  // For each trial/phrase
  let h = 20;
  for (i = 0; i < 2; i++, h += 40) {
    text(
      "Target phrase " + (i + 1) + ": " + phrases[i],
      width / 2,
      height / 2 + h
    );
    text(
      "User typed " + (i + 1) + ": " + entered[i],
      width / 2,
      height / 2 + h + 20
    );
  }

  text("Raw WPM: " + wpm.toFixed(2), width / 2, height / 2 + h + 20);
  text(
    "Freebie errors: " + freebie_errors.toFixed(2),
    width / 2,
    height / 2 + h + 40
  );
  text("Penalty: " + penalty.toFixed(2), width / 2, height / 2 + h + 60);
  text(
    "WPM with penalty: " + wpm_w_penalty.toFixed(2),
    width / 2,
    height / 2 + h + 80
  );
  text ("CPS: " + CPS.toFixed(2), width / 2, height / 2 + h+100);

  // Saves results (DO NOT CHANGE!)
  let attempt_data = {
    project_from: GROUP_NUMBER,
    assessed_by: student_ID,
    attempt_completed_by: timestamp,
    attempt: attempt,
    attempt_duration: attempt_duration,
    raw_wpm: wpm,
    freebie_errors: freebie_errors,
    penalty: penalty,
    wpm_w_penalty: wpm_w_penalty,
    cps: CPS,
  };

  // Send data to DB (DO NOT CHANGE!)
  if (BAKE_OFF_DAY) {
    // Access the Firebase DB
    if (attempt === 0) {
      firebase.initializeApp(firebaseConfig);
      database = firebase.database();
    }

    // Add user performance results
    let db_ref = database.ref("G" + GROUP_NUMBER);
    db_ref.push(attempt_data);
  }
}

// Is invoked when the canvas is resized (e.g., when we go fullscreen)
function windowResized() {
  resizeCanvas(windowWidth, windowHeight);
  let display = new Display({ diagonal: display_size }, window.screen);

  // DO NO CHANGE THESE!
  PPI = display.ppi; // calculates pixels per inch
  PPCM = PPI / 2.54; // calculates pixels per cm
  FINGER_SIZE = int(11 * PPCM);
  FINGER_OFFSET = int(0.8 * PPCM);
  ARM_LENGTH = int(19 * PPCM);
  ARM_HEIGHT = int(11.2 * PPCM);

  ARROW_SIZE = int(2.2 * PPCM);

  firstRowSpacing = (4 * PPCM) / 10;
  secondRowSpacing = (4 * PPCM) / 9;
  thirdRowSpacing = (4 * PPCM) / 7;
  fourthRowSpacing = (4 * PPCM) / 3;
  verticalSpacing = (3 * PPCM) / 4;
  displayedArea = 0;

  // Starts drawing the watch immediately after we go fullscreen (DO NO CHANGE THIS!)
  draw_finger_arm = true;
  attempt_start_time = millis();
}
