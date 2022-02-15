## Audio Engine

The new FFNx Audio Engine is completely configurable by you! Every audio layer can now have its own `config.toml` file, which through it you can customize the Audio Engine behavior when a track is going to be played.

The `config.toml` MUST BE within the relative configured `external_*_path` entry in the [FFNx.toml](misc/FFNx.toml) file.
For example, for the SFX layer on default configuration, the file should be placed in `sfx/config.toml`.

Feel free to check the example configuration included in each FFNx release.

### Audio Engine Layers

The current supported Audio Engine layers are:

- [SFX](misc/FFNx.SFX.toml): in-game audio sound effects ( menu cursor sound, battle sword slash sound, etc. )
- [Music](misc/FFNx.music.toml): in-game audio music ( world theme, field theme, etc. )
- Voice: in-game audio voice acting ( dialog voice acting )
- Ambient: in-game audio atmosphere effects

## Music List

> Courtesy of [myst6re](https://github.com/myst6re)

Following below, you can find a list of file names that you can replace when using the external music layer, depending on the game you target.

Those filenames can be used directly within the relative Music folder and also within the Music TOML configuration file.

### FF7

| File name | Description                                    |
| --------- | ---------------------------------------------- |
| aseri     | Hurry!                                         |
| aseri2    | Hurry Faster!                                  |
| ayasi     | Lurking in the Darkness                        |
| barret    | Barret's Theme                                 |
| bat       | Fighting                                       |
| bee       | Honeybee Manor                                 |
| bokujo    | Farm Boy                                       |
| boo       | Life Stream                                    |
| cannon    | The Makou Cannon Fires                         |
| canyon    | Cosmo Canyon                                   |
| cephiros  | Those Chosen by the Planet                     |
| chase     | Crazy Motorcycle                               |
| chu       | Still More Fighting                            |
| chu2      | J-E-N-O-V-A                                    |
| cinco     | Cinco de Chocobo                               |
| cintro    | Those chosen by the planet (intro)             |
| comical   | Comical (deleted song)                         |
| condor    | Fortress of the Condor                         |
| corel     | Mining Town                                    |
| corneo    | Don of the Slums                               |
| costa     | Costa del Sol                                  |
| crlost    | Tango of Tears                                 |
| crwin     | A Great Success                                |
| date      | Interrupted by Fireworks                       |
| dokubo    | Underneath the Rotting Pizza                   |
| dun2      | Chasing the Black-Caped Man                    |
| earis     | Aerith's Theme                                 |
| earislo   | Flowers Blooming in the Church                 |
| elec      | Electric de Chocobo                            |
| fan2      | Fanfare                                        |
| fanfare   | Fanfare (alternate)                            |
| fiddle    | Fiddle de Chocobo                              |
| fin       | World Crisis                                   |
| geki      | Debut                                          |
| gold1     | Gold Saucer                                    |
| guitar2   | On the Other Side of the Mountain              |
| gun       | ShinRa Army Wages a Full-Scale Attack          |
| hen       | Who Am I                                       |
| hiku      | Highwind Takes to the Skies                    |
| horror    | Trail of Blood                                 |
| iseki     | You Can Hear the Cry of the Planet             |
| jukai     | Forest Temple                                  |
| junon     | Off the Edge of Despair                        |
| jyro      | Steal the Tiny Bronco!                         |
| ketc      | Cait Sith's Theme                              |
| kita      | The Great Northern Cave                        |
| kurai     | Anxious Heart                                  |
| lb1       | The Birth of God                               |
| lb2       | A One-Winged Angel                             |
| ld        | Judgement Day                                  |
| makoro    | Makou Reactor                                  |
| mati      | Ahead on Our Way                               |
| mekyu     | Reunion                                        |
| mogu      | Highwind Takes to the Skies (Moggle version)   |
| mura1     | Parochial Town                                 |
| nointro   | Sephiroth intro                                |
| oa        | Opening - Bombing Mission                      |
| ob        | Bombing Mission                                |
| odds      | Racing Chocobos                                |
| over2     | Continue                                       |
| parade    | Rufus' Welcoming Ceremony                      |
| pj        | Jenova Absolute                                |
| pre       | Prelude                                        |
| red       | Red XIII's Theme                               |
| rhythm    | Turk's Theme                                   |
| riku      | unknown                                        |
| ro        | The Countdown Begins                           |
| rocket    | Oppressed People                               |
| roll      | Staff Roll                                     |
| rukei     | Sandy Badlands                                 |
| sadbar    | Mark of the Traitor                            |
| sadsid    | Sending a Dream Into the Universe              |
| sea       | A Secret, Sleeping in the Deep Sea             |
| seto      | Great Warrior                                  |
| si        | unknown                                        |
| sid2      | Cid's Theme                                    |
| sido      | It's Difficult to Stand on Both Feet, Isn't It |
| siera     | If You Open Your Heart                         |
| sinra     | ShinRa Corporation                             |
| sinraslo  | Infiltrating ShinRa Tower                      |
| snow      | Buried in the Snow                             |
| ta        | FF VII Main Theme                              |
| tb        | FF VII Main Theme (alternate)                  |
| tender    | Holding My Thoughts In My Heart                |
| tifa      | Tifa's Theme                                   |
| tm        | On That Day, 5 Years Ago                       |
| utai      | Wutai                                          |
| vincent   | The Nightmare's Beginning                      |
| walz      | Waltz de Chocobo                               |
| weapon    | Weapon Raid                                    |
| wind      | Wind                                           |
| yado      | Good Night, Until Tomorrow                     |
| yufi      | Descendant of Shinobi                          |
| yufi2     | Stolen Materia                                 |
| yume      | Who Are You                                    |
| heart     |                                                |
| sato      |                                                |
| sensui    |                                                |
| wind      |                                                |

### FF8

| File name    | Description                            |
| ------------ | -------------------------------------- |
| lose         | The Loser                              |
| win          | The Winner                             |
| run          | Never Look Back                        |
| battle       | Don't Be Afraid                        |
| end          | Dead End                               |
| antena       | Starting Up                            |
| waiting      | Intruders                              |
| kani         | Don't Be Afraid (Alternate)            |
| battle2      | Force Your Way                         |
| Parade2      | Fithos Lusec Wecos Vinosec (Variation) |
| fuan2        | Unrest                                 |
| march2       | The Stage Is Set                       |
| joriku       | The Landing                            |
| julia        | Love Grows                             |
| waltz        | Waltz For the Moon                     |
| friend       | Ami                                    |
| dangeon      | Find Your Way                          |
| pianosol     | Julia                                  |
| Parade       | FITHOS LUSEC WECOS VINOSEC             |
| march1       | SeeD                                   |
| himitsu      | Tell Me                                |
| garden       | Balamb Garden                          |
| fuan         | Fear                                   |
| polka2       | Dance with the Balamb-Fish             |
| anthem       | Cactus Jack (Galbadian Anthem)         |
| m7f5         | The Mission                            |
| majo         | Succession of Witches                  |
| field        | Blue Fields                            |
| guitar       | Breezy                                 |
| resistan     | Timber Owls                            |
| kaiso        | Fragments of Memories                  |
| horizon      | Fisherman's Horizon                    |
| master       | Heresy                                 |
| rinoa        | My Mind                                |
| travia       | Where I Belong                         |
| antena2      | Starting Up (Variation)                |
| truth        | Truth                                  |
| jail         | Trust Me                               |
| gargarde     | Galbadia Garden                        |
| timber       | Martial Law                            |
| garbadia     | Under Her Control                      |
| pinch        | Only a Plank Between One and Perdition |
| scene1       | Junction                               |
| pub          | Roses And Wine                         |
| bat3         | The Man With the Machine Gun           |
| stage        | A Sacrifice                            |
| choco        | Odeka ke Chocobo                       |
| white        | Drifting                               |
| majomv       | Wounded                                |
| musho        | Jailed                                 |
| missile      | Retaliation                            |
| enzetu       | The Oath                               |
| card         | Shuffle Or Boogie                      |
| gomon        | Rivals                                 |
| soto         | Blue Sky                               |
| majobat      | Premonition                            |
| Gar3         | Galbadia Garden (Variation)            |
| Bossbat2     | Maybe I'm a Lion                       |
| lasdun       | The Castle                             |
| gafly        | Movin'                                 |
| demo         | Overture                               |
| spy          | The Spy                                |
| mods         | Mods de Chocobo                        |
| salt         | The Salt Flats                         |
| alien        | Residents                              |
| sekichu      | Lunatic Pandora                        |
| esta         | Silence and Motion                     |
| moonmv       | Tears of the Moon                      |
| hikutei      | Ride On                                |
| bossbat1     | The Legendary Beast                    |
| rag1         | Slide Show Part 1                      |
| rag2         | Slide Show Part 2                      |
| lasboss      | The Extreme (no effects)               |
| lasbossintro | The Extreme (intro)                    |
| keisho       | The Successor                          |
| ashuku       | Compression of Time                    |
| joriku2      | The Landing                            |
| combat       |                                        |
| funsui       |                                        |
| ante         |                                        |
| wind         |                                        |
| Flangchorus  |                                        |
| dubchorus    |                                        |
| Solochorus   |                                        |
| Femalechorus |                                        |
| chorus       |                                        |
| reet         |                                        |
| soyo         |                                        |
| rouka        |                                        |
| night        |                                        |
| sea          |                                        |
| train        |                                        |
| mdmotor      |                                        |
| laswhite     |                                        |
| lasbl        |                                        |
