# Ai changes

Here is described the changes made to the AI system in FF8.
The main change is the addition of new target values for the AI opcode "target" (0x04), which allows for more complex targeting options during battles.

# Target opcode

FF8 AI use opcode, one we called "target" (0x04). Target defines what will be the next target for next attack.
In vanilla you can either target all, or target one, following the below table of possible values:

| Value       | Meaning                                                              | How it resolves                                                      |
|-------------|----------------------------------------------------------------------|----------------------------------------------------------------------|
| **0-10**    | Specific **character** by identity                                   | com-scan (matches character id)                                      |
| **11-15**   | Spare character ids                                                  | com-scan → no match → no target                                      |
| **16-159**  | Specific **enemy** by com id — `c0m000`-`c0m143` (value = c0m# + 16) | com-scan                                                             |
| **160-199** | `c0m144`-`c0m183`                                                    | com-scan → dead in vanilla; **live only with the c0m144-199 unlock** |
| **200**     | `SELF`                                                               | the acting monster                                                   |
| **201**     | `RANDOM ENEMY`                                                       | one random character (side 0-2)                                      |
| **202**     | `RANDOM ALLY`                                                        | one random monster (side 3-7)                                        |
| **203**     | `LAST ATTACKER`                                                      | whoever last hit the actor                                           |
| **204**     | `ALL ENEMIES`                                                        | all characters (0+1+2), several-flag set                             |
| **205**     | `ALL ALLIES`                                                         | all monsters (3-7), several-flag set                                 |
| **206**     | `EVERYONE`                                                           | all 8 slots, several-flag set                                        |
| **207**     | `RANDOM NONSELF ALLY`                                                | a random monster that isn't the actor                                |
| **208**     | `RANDOM ENEMY EACH HIT`                                              | re-rolls a character per hit                                         |
| **209**     | `NEW ALLY`                                                           | the slot of a monster just entering combat                           |
| **210-215** | `c0m194`-`c0m199`                                                    | com-scan → dead in vanilla; live with the unlock                     |
| **216-219** | `c0m200`-`c0m203` (never exist)                                      | com-scan → **always dead**                                           |
| **220**     | `varA MASK`                                                          | the slot bitmask held in AI variable A                               |
| **221**     | `varB MASK`                                                          | the slot bitmask held in AI variable B                               |
| **222**     | `varC MASK`                                                          | the slot bitmask held in AI variable C                               |
| **223**     | `varD MASK`                                                          | the slot bitmask held in AI variable D                               |
| **224**     | `varE MASK`                                                          | the slot bitmask held in AI variable E                               |
| **225**     | `varF MASK`                                                          | the slot bitmask held in AI variable F                               |
| **226**     | `varG MASK`                                                          | the slot bitmask held in AI variable G                               |
| **227**     | `varH MASK`                                                          | the slot bitmask held in AI variable H                               |

This leaves a big gap of values between 228 and 255. This PR aims to use it in order to have the following target possibilities:

| Val | Slots | Val | Slots | Val | Slots |
|-----|-------|-----|-------|-----|-------|
| 228 | 0+1   | 235 | 1+3   | 242 | 2+6   |
| 229 | 0+2   | 236 | 1+4   | 243 | 3+4   |
| 230 | 0+3   | 237 | 1+5   | 244 | 3+5   |
| 231 | 0+4   | 238 | 1+6   | 245 | 3+6   |
| 232 | 0+5   | 239 | 2+3   | 246 | 4+5   |
| 233 | 0+6   | 240 | 2+4   | 247 | 4+6   |
| 234 | 1+2   | 241 | 2+5   | 248 | 5+6   |

Slot are the different chara/enemies, so slot 0 is the first chara till slot 2, then slot 3 is first monster till slot 6.

One way to implement this better would be to manage bitmask in varA to handle two bit set (atm it handle only 1).
But this is way more complex to do that just adding values.
Just putting it there if anytime in the futur we need more values for other reason, this is one possible gain.
