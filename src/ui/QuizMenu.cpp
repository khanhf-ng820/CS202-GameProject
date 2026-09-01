#include "QuizMenu.h"
#include "UIHelpers.h"
#include "AudioManager.h"
#include "ProfileManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

static std::string FormatMoney(int amount) {
    std::string s = std::to_string(amount);
    int n = (int)s.length() - 3;
    while (n > 0) {
        s.insert(n, ",");
        n -= 3;
    }
    return "$" + s;
}

QuizMenu::QuizMenu(Resources& res)
    : m_res(res), m_state(QuizState::Rules), m_currentQuestionIndex(0), m_score(0), m_coinsEarned(0), m_totalPlayerCoins(ProfileManager::GetInstance().GetActiveProfile().coins) {
    
    // Load background, coin bank, and main menu button textures
    m_bg            = res.GetTexture("CHALLENGE_BACKGROUND");
    m_coinBank      = res.GetTexture("COINBANK");
    m_closeBtn      = res.GetTexture("ALMANAC_INDEXBUTTON");
    m_closeBtnHl    = res.GetTexture("ALMANAC_INDEXBUTTONHIGHLIGHT");

    // Load Crazy Dave reanim
    std::string davePath = res.GetAssetPath("assets/reanim/CrazyDave.reanim");
    ReanimDefinition daveDef = res.LoadReanim(davePath);
    m_crazyDave.SetResources(daveDef, res);
    m_crazyDave.SetAnimation("anim_idle");

    // Load fonts
    std::string hotPng = res.GetAssetPath("assets/data/HouseofTerror28.png");
    std::string hotTxt = res.GetAssetPath("assets/data/HouseofTerror28.txt");
    m_fontTitle.Load(hotPng, hotTxt);

    std::string briannePng = res.GetAssetPath("assets/data/_BrianneTod16.png");
    std::string brianneTxt = res.GetAssetPath("assets/data/BrianneTod16.txt");
    m_fontQuestion.Load(briannePng, brianneTxt);

    std::string todcraftPng = res.GetAssetPath("assets/data/DwarvenTodcraft24.png");
    std::string todcraftTxt = res.GetAssetPath("assets/data/DwarvenTodcraft24.txt");
    m_fontOption.Load(todcraftPng, todcraftTxt);

    std::string continuumPng = res.GetAssetPath("assets/data/_ContinuumBold14.png");
    std::string continuumTxt = res.GetAssetPath("assets/data/ContinuumBold14.txt");
    m_fontStats.Load(continuumPng, continuumTxt);

    loadQuestions();
}

void QuizMenu::loadQuestions() {
    m_allQuestions = {
        // 1-10: Basic Plants & Day
        { "How much Sun does a standard SunFlower cost to plant?", { "A. 25 Sun", "B. 50 Sun", "C. 75 Sun", "D. 100 Sun" }, 1, "Wabby wabbo! You know your economy!" },
        { "Which plant shoots peas backwards as well as forwards?", { "A. Repeater", "B. Threepeater", "C. Split Pea", "D. Gatling Pea" }, 2, "Two heads are crazier than one!" },
        { "Which tall defense plant blocks pole-vaulting zombies from jumping?", { "A. Wall-nut", "B. Tall-nut", "C. Pumpkin", "D. Squash" }, 1, "No vaulting over this giant vegetable!" },
        { "What unusual item does Crazy Dave always wear on his head?", { "A. Traffic Cone", "B. Metal Bucket", "C. Cooking Pot", "D. Flower Pot" }, 2, "BECAUSE I'M CRAAAZY!!" },
        { "Which mushroom is used to wake up nocturnal mushrooms during the day?", { "A. Coffee Bean", "B. Hypno-shroom", "C. Sun-shroom", "D. Doom-shroom" }, 0, "A fresh cup of hot brew keeps the shrooms awake!" },
        { "Which plant immediately squashes the first zombie that steps close to it?", { "A. Chomper", "B. Potato Mine", "C. Jalapeno", "D. Squash" }, 3, "HMMMPH! Splat goes the zombie!" },
        { "What happens to a normal Pea when it flies through a Torchwood?", { "A. Becomes an Ice Pea", "B. Turns into Fire Pea (2x DMG)", "C. Explodes in midair", "D. Speeds up 3x" }, 1, "Double spicy firepower! Extra crispy!" },
        { "How many peas does a Gatling Pea unleash in a single rapid burst?", { "A. 2 Peas", "B. 3 Peas", "C. 4 Peas", "D. 5 Peas" }, 2, "Rattattat! Maximum garden suppression!" },
        { "Which zombie speeds up frantically once its newspaper is destroyed?", { "A. Football Zombie", "B. Newspaper Zombie", "C. Pole Vaulter", "D. Flag Zombie" }, 1, "Never disturb a zombie doing its morning crossword!" },
        { "What explosive plant turns an entire vertical row of zombies to ash?", { "A. Cherry Bomb", "B. Potato Mine", "C. Jalapeno", "D. Doom-shroom" }, 2, "Spicy jalapeno peppers! Caliente!" },

        // 11-20: Night & Fog
        { "How much Sun does a free Puff-shroom cost to plant?", { "A. 0 Sun", "B. 25 Sun", "C. 50 Sun", "D. 75 Sun" }, 0, "Free mushrooms! Best deal in the universe!" },
        { "Which mushroom explodes into a massive crater leaving a dead tile?", { "A. Ice-shroom", "B. Doom-shroom", "C. Sun-shroom", "D. Fume-shroom" }, 1, "DOOOOOM!! Watch out for the giant hole!" },
        { "Which plant clears all fog and blows away flying Balloon Zombies?", { "A. Cactus", "B. Plantern", "C. Blover", "D. Split Pea" }, 2, "Whooosh! Gone with the wind!" },
        { "Which mushroom temporarily freezes all zombies on the entire screen?", { "A. Ice-shroom", "B. Doom-shroom", "C. Hypno-shroom", "D. Fume-shroom" }, 0, "Freeze! Chill out, zombies!" },
        { "What plant is required before you can place terrestrial plants on water?", { "A. Sea-shroom", "B. Lily Pad", "C. Tangle Kelp", "D. Cattail" }, 1, "Lily pads keep your garden afloat!" },
        { "Which plant hypnotizes a zombie to turn around and fight for your lawn?", { "A. Hypno-shroom", "B. Chomper", "C. Scaredy-shroom", "D. Garlic" }, 0, "Now they are on OUR side!" },
        { "What is the special ability of the Grave Buster?", { "A. Eats Gravestones", "B. Squashes zombies", "C. Shoots rocks", "D. Produces sun" }, 0, "Nom nom nom! Goodbye pesky graves!" },
        { "Which plant illuminates the fog in a wide circular area around itself?", { "A. Torchwood", "B. Plantern", "C. SunFlower", "D. Starfruit" }, 1, "Let there be light in the spooky mist!" },
        { "Which plant can shoot sharp thorns in 5 different directions?", { "A. Cactus", "B. Starfruit", "C. Threepeater", "D. Gloom-shroom" }, 1, "Shooting stars all across the lawn!" },
        { "Which plant drags a single swimming zombie underwater to defeat it?", { "A. Lily Pad", "B. Tangle Kelp", "C. Sea-shroom", "D. Cattail" }, 1, "Pulled down into the deep blue depths!" },

        // 21-30: Roof & Defense
        { "What must be placed on roof tiles before planting vegetables?", { "A. Lily Pad", "B. Flower Pot", "C. Soil Patch", "D. Sod Roll" }, 1, "Gotta have a pot on the shingles!" },
        { "Which catapult plant throws heavy melons that deal splash damage?", { "A. Cabbage-pult", "B. Kernel-pult", "C. Melon-pult", "D. Cob Cannon" }, 2, "Juicy heavy melon impact!" },
        { "Which catapult plant occasionally lobs butter to immobilize a zombie?", { "A. Cabbage-pult", "B. Kernel-pult", "C. Melon-pult", "D. Winter Melon" }, 1, "Butter on the head! Slippery goodness!" },
        { "Which plant diverts zombies to adjacent lanes when they bite into it?", { "A. Garlic", "B. Wall-nut", "C. Chomper", "D. Spikeweed" }, 0, "PHEW! Stinky garlic breath reroutes traffic!" },
        { "Which plant protects nearby crops from Bungee Zombies and catapults?", { "A. Umbrella Leaf", "B. Pumpkin", "C. Tall-nut", "D. Cactus" }, 0, "Bungee umbrella bounce! Blocked!" },
        { "Which shell plant can be planted directly on other plants to armor them?", { "A. Wall-nut", "B. Pumpkin", "C. Tall-nut", "D. Flower Pot" }, 1, "Jack-o-lantern armor protects your peas!" },
        { "Which plant continuously produces silver and gold coins?", { "A. SunFlower", "B. Twin Sunflower", "C. Marigold", "D. Gold Magnet" }, 2, "Money money money! Cha-ching!" },
        { "Which floor hazard damages walking zombies and cannot be eaten?", { "A. Spikeweed", "B. Potato Mine", "C. Garlic", "D. Tangle Kelp" }, 0, "Ouch! Spikes under their rotten boots!" },
        { "How many Zamboni vehicles can a standard Spikeweed pop before breaking?", { "A. 1 Zamboni", "B. 2 Zambonis", "C. 3 Zambonis", "D. Unlimited" }, 0, "One spike pops one tire, but it destroys the plant!" },
        { "How many vehicles or Gargantuar smashes can a Spikerock survive?", { "A. 1 Hit", "B. 3 Hits", "C. 5 Hits", "D. 9 Hits" }, 1, "Hard as rock! Survives 3 heavy crushes!" },

        // 31-40: Upgrades & Specials
        { "Which upgrade plant shoots homing spikes at any lane or balloon?", { "A. Cattail", "B. Gatling Pea", "C. Gloom-shroom", "D. Cob Cannon" }, 0, "Meow! Homing spikes never miss!" },
        { "Which upgrade plant fires heavy explosive corn cobs at any target?", { "A. Melon-pult", "B. Cob Cannon", "C. Kernel-pult", "D. Doom-shroom" }, 1, "Tactical corn missile inbound! BOOM!" },
        { "What does the Twin Sunflower produce each time it generates sun?", { "A. 25 Sun", "B. 50 Sun (2 tokens)", "C. 75 Sun", "D. 100 Sun" }, 1, "Double the sunshine, double the power!" },
        { "Which upgrade mushroom attacks all 8 adjacent tiles around itself?", { "A. Fume-shroom", "B. Gloom-shroom", "C. Ice-shroom", "D. Doom-shroom" }, 1, "360-degree mushroom foam blast!" },
        { "Which plant copies and becomes a duplicate of any chosen plant in your deck?", { "A. Imitater", "B. Hypno-shroom", "C. Twin Sunflower", "D. Coffee Bean" }, 0, "Mime potato! Now you can bring two of the same!" },
        { "What color does an Imitater appear as before turning into the copied plant?", { "A. Rainbow", "B. Gray (Black & White)", "C. Golden", "D. Transparent" }, 1, "Classic monochrome potato mime style!" },
        { "Which upgrade plant upgrades Melon-pult to slow down and freeze zombies?", { "A. Ice-shroom", "B. Snow Pea", "C. Winter Melon", "D. Gatling Pea" }, 2, "Sub-zero frozen watermelon splash!" },
        { "What special item does the Magnet-shroom steal from Football Zombies?", { "A. Football Helmet", "B. Football", "C. Cleats", "D. Shoulder Pads" }, 0, "Yank! No more sturdy metal helmet!" },
        { "What happens when an Ice Pea passes through a Torchwood?", { "A. Becomes Super Ice Pea", "B. Melts into a Normal Pea", "C. Extinguishes Torchwood", "D. Explodes" }, 1, "Fire and ice cancel out! Normal pea!" },
        { "Which plant stretches upwards to pop flying Balloon Zombies?", { "A. Blover", "B. Cactus", "C. Tall-nut", "D. Cattail" }, 1, "Stretch up high and pop that balloon!" },

        // 41-50: Zombies & Armor
        { "How many regular pea hits does it take to destroy a Conehead's cone?", { "A. 10 hits", "B. 18 hits", "C. 28 hits", "D. 40 hits" }, 2, "Traffic safety cone provides 28 regular pea hits of armor!" },
        { "Which metal object gives the Buckethead Zombie massive endurance?", { "A. Iron Bucket", "B. Trash Can", "C. Tin Pot", "D. Saucepan" }, 0, "Heavy duty metal bucket! Magnet-shroom's favorite target!" },
        { "Which zombie summons four Backup Dancers around itself on the lawn?", { "A. Dancing Zombie", "B. Flag Zombie", "C. Jackson Zombie", "D. Bungee Zombie" }, 0, "Thriller moves on the garden dancefloor!" },
        { "Which zombie drives an ice-smoothing machine leaving an icy trail?", { "A. Bobsled Zombie", "B. Zamboni Zombie", "C. Catapult Zombie", "D. Digger Zombie" }, 1, "Zamboni machine! Melts with Jalapeno or Spikeweed!" },
        { "Which zombie team spawns and slides quickly across ice trails?", { "A. Bobsled Team", "B. Dolphin Rider", "C. Snorkel Zombie", "D. Ducky Tube" }, 0, "4 zombies in one bobsled sled!" },
        { "Which zombie jumps over pool defenses riding a pet sea mammal?", { "A. Snorkel Zombie", "B. Dolphin Rider Zombie", "C. Ducky Tube Zombie", "D. Bungee Zombie" }, 1, "High jump over your lily pads!" },
        { "Which zombie winds up a music box before exploding a 3x3 area?", { "A. Jack-in-the-Box Zombie", "B. Digger Zombie", "C. Balloon Zombie", "D. Pogo Zombie" }, 0, "Pop Goes the Weasel... BOOM!" },
        { "Which zombie tunnels underground to the extreme left side of the lawn?", { "A. Digger Zombie", "B. Miner Zombie", "C. Grave Zombie", "D. Pogo Zombie" }, 0, "Digs under your defenses and walks backwards!" },
        { "Which zombie bounces on a pogo stick to bypass ground plants?", { "A. Pole Vaulter", "B. Pogo Zombie", "C. Dolphin Rider", "D. Bungee Zombie" }, 1, "Boing boing boing! Magnet-shroom steals that pogo!" },
        { "Which rare zombie only appears in Fog levels and drops diamonds?", { "A. Yeti Zombie", "B. Gold Zombie", "C. Crazy Zombie", "D. Treasure Zombie" }, 0, "The elusive Zombie Yeti! Catch him before he flees!" },

        // 51-60: Roof & Bosses
        { "Which airborne zombie drops from the sky to steal your plants?", { "A. Balloon Zombie", "B. Bungee Zombie", "C. Catapult Zombie", "D. Gargantuar" }, 1, "YEEHAW! Protect with Umbrella Leaf!" },
        { "Which zombie carries a metal ladder to climb over defensive nuts?", { "A. Ladder Zombie", "B. Pole Vaulter", "C. Pogo Zombie", "D. Digger Zombie" }, 0, "Scales right over your Wall-nuts!" },
        { "Which zombie drives a machine that flings basketballs at your backline?", { "A. Zamboni", "B. Catapult Zombie", "C. Gargantuar", "D. Dr. Zomboss" }, 1, "Basketball barrage from the far right!" },
        { "What gigantic zombie crushes plants instantly with a heavy telephone pole?", { "A. Gargantuar", "B. Super Zombie", "C. Dr. Zomboss", "D. Titan Zombie" }, 0, "HUGE behemoth! Takes 2 instant kills to defeat!" },
        { "What small zombie does the Gargantuar throw deep into your defenses?", { "A. Imp", "B. Mini Zombie", "C. Baby Zombie", "D. Ducky Zombie" }, 0, "Tosses the little Imp into your third column!" },
        { "Who is the final boss of Adventure Mode piloting the giant robot Zombot?", { "A. Dr. Edgar Zomboss", "B. Crazy Dave", "C. King Zombie", "D. Professor Brain" }, 0, "Dr. Edgar George Zomboss, leader of the undead!" },
        { "What plant should you plant in front of Dr. Zomboss's Ice Ball attack?", { "A. Ice-shroom", "B. Jalapeno", "C. Chomper", "D. Squash" }, 1, "Hot Jalapeno fire melts the giant ice sphere!" },
        { "What plant should you plant in front of Dr. Zomboss's Fire Ball attack?", { "A. Ice-shroom", "B. Snow Pea", "C. Winter Melon", "D. Water Pot" }, 0, "Ice-shroom freeze extinguishes the massive fireball!" },
        { "What vehicle does Dr. Zomboss drop from above to crush a 2x3 area?", { "A. RV / Camper Van", "B. Police Car", "C. School Bus", "D. Tractor" }, 0, "Watch out! Crushed by Crazy Dave's spare RV!" },
        { "What happens to straight-shooting plants on the left side of the roof?", { "A. They miss due to slant", "B. They deal 2x damage", "C. They cannot shoot", "D. They shoot backwards" }, 0, "The roof pitch blocks low peas! Use catapults!" },

        // 61-70: Game Currency & Economics
        { "How much cash value is a shiny Gold Coin worth in Crazy Dave's shop?", { "A. $10", "B. $50", "C. $100", "D. $250" }, 0, "Ten bucks! Adds up fast in the coin bank!" },
        { "How much cash value is a Silver Coin worth?", { "A. $1", "B. $5", "C. $10", "D. $20" }, 1, "Five bucks a pop!" },
        { "How much cash value does a rare sparkling Diamond award when picked up?", { "A. $500", "B. $1,000", "C. $2,500", "D. $5,000" }, 1, "A whopping $1,000 cold hard cash!" },
        { "What is the maximum number of seed packet slots you can purchase in shop?", { "A. 8 Slots", "B. 9 Slots", "C. 10 Slots", "D. 12 Slots" }, 2, "Ten slots max! Build your ultimate deck!" },
        { "What is the price of the 7th Seed Slot in Crazy Dave's Twiddydinkies?", { "A. $750", "B. $1,000", "C. $5,000", "D. $7,500" }, 0, "Starts at $750 for slot number 7!" },
        { "What is the ultimate price of the 10th and final Seed Slot in the shop?", { "A. $10,000", "B. $20,000", "C. $50,000", "D. $80,000" }, 3, "A hefty $80,000 for maximum versatility!" },
        { "What last-resort lawn mower defense item is unlocked for the pool lanes?", { "A. Pool Cleaner", "B. Roof Cleaner", "C. Lawn Mower", "D. Grave Buster" }, 0, "Drives right across the water chopping up zombies!" },
        { "How many sun points does a normal falling sun from the sky provide?", { "A. 25 Sun", "B. 50 Sun", "C. 75 Sun", "D. 100 Sun" }, 0, "Standard golden 25 sun drop!" },
        { "How much sun does a small baby Sun-shroom produce before growing big?", { "A. 10 Sun", "B. 15 Sun", "C. 20 Sun", "D. 25 Sun" }, 1, "15 sun initially, then grows to produce 25!" },
        { "How many seconds does it take for a planted Potato Mine to arm itself?", { "A. ~5 seconds", "B. ~14 seconds", "C. ~30 seconds", "D. Instant" }, 1, "Takes ~14 seconds to pop up and arm!" },

        // 71-80: Special Interactions & Lore
        { "What is Crazy Dave's greeting in almost every conversation?", { "A. Hello neighbor!", "B. Greetings!", "C. Wabby Wabbo / Craaazy!", "D. Brainz!" }, 2, "Wabby wabbo! Because I'm CRAAAZY!" },
        { "What is the name of Crazy Dave's car in his backyard shop?", { "A. Penny", "B. Corvette", "C. Twiddy", "D. Rusty" }, 0, "Penny the talking car in PvZ lore!" },
        { "What delicious food does Crazy Dave sell in his shop for $1,000?", { "A. Taco", "B. Bacon", "C. Hot Dog", "D. Pizza" }, 1, "Crispy bacon! Dave loves his bacon!" },
        { "What tree can you grow in the backyard that dispenses tips and codes?", { "A. Tree of Life", "B. Tree of Wisdom", "C. Oak of Knowledge", "D. Beanstalk" }, 1, "Tree of Wisdom! Grows hundreds of feet tall!" },
        { "At what Tree height does the code 'daisies' make zombies leave flowers?", { "A. 100 feet", "B. 500 feet", "C. 1,000 feet", "D. 50 feet" }, 0, "100 feet tall unlocks daisy flower spawns!" },
        { "What happens when you type the cheat code 'mustache' during gameplay?", { "A. Zombies grow mustaches", "B. Plants wear hats", "C. Dave dances", "D. Coins double" }, 0, "Every single zombie rocks a handsome mustache!" },
        { "What happens when you type the cheat code 'future' during gameplay?", { "A. Futuristic music", "B. Zombies wear sci-fi shades", "C. Laser peas", "D. Jetpacks" }, 1, "Zombies get sleek cyber sunglasses!" },
        { "Which Zen Garden creature sleeps during day and collects coins for you?", { "A. Stinky the Snail", "B. Mojo the Dog", "C. Penny", "D. Gargantuar" }, 0, "Stinky the Snail! Feed him chocolate to speed him up!" },
        { "What food item can you give to Stinky the Snail to keep him awake 1 hour?", { "A. Coffee Bean", "B. Chocolate", "C. Sugar", "D. Bacon" }, 1, "Sweet chocolate rush keeps Stinky gliding fast!" },
        { "How many plants can you select in your active seed bank in initial levels?", { "A. 4 Plants", "B. 5 Plants", "C. 6 Plants", "D. 7 Plants" }, 2, "You start Adventure with 6 seed packet slots!" },

        // 81-90: Minigames & Puzzles
        { "In Wall-nut Bowling, what happens when an Explode-o-nut hits a zombie?", { "A. Bounces 3 times", "B. Explodes in 3x3 radius", "C. Pushes zombie back", "D. Freezes lane" }, 1, "Red nut goes KABOOM on first contact!" },
        { "In Wall-nut Bowling, how does a standard Wall-nut move after hitting a zombie?", { "A. Stops moving", "B. Bounces diagonally", "C. Explodes", "D. Rolls faster" }, 1, "Bounces off at a 45-degree angle!" },
        { "In Wall-nut Bowling, which gigantic nut bowls through all zombies in lane?", { "A. Giant Wall-nut", "B. Tall-nut", "C. Explode-o-nut", "D. Pumpkin" }, 0, "Giant Wall-nut flattens entire lanes like a steamroller!" },
        { "In Vasebreaker puzzle mode, what color are vases with plant seed packets?", { "A. Brown ceramic", "B. Green with plant leaf", "C. Red", "D. Golden" }, 1, "Green leaf vases guarantee helpful plants!" },
        { "In the I, Zombie puzzle mode, what role does the player take?", { "A. Planting defenses", "B. Placing zombies to eat plants", "C. Controlling Dave", "D. Bowling nuts" }, 1, "You are the zombies trying to reach the brains!" },
        { "In Zombiquarium, what aquatic zombie do you feed brains to produce sun?", { "A. Ducky Tube Zombie", "B. Snorkel Zombie", "C. Dolphin Rider", "D. Yeti" }, 0, "Feed the cute little pet Snorkel/Ducky zombies brains!" },
        { "Which minigame features shrinking plants and tiny fast-moving mini zombies?", { "A. Big Trouble Little Zombie", "B. Portal Combat", "C. Invisighoul", "D. Column Like You See 'Em" }, 0, "Tiny mini zombies scurrying across the lawn!" },
        { "Which minigame features magical teleportation portals across the lawn?", { "A. Portal Combat", "B. Slot Machine", "C. Whack a Zombie", "D. Bobsled Bonanza" }, 0, "Shoot peas through portals to warp lanes!" },
        { "In the Slot Machine minigame, what sun target must you reach to win?", { "A. 2,000 Sun", "B. 3 Diamonds", "C. 5 Sunflowers", "D. 10 Mowers" }, 0, "Reach 2,000 sun by spinning the slot reel!" },
        { "In Whack a Zombie, what tool do you use to smash zombies popping from graves?", { "A. Shovel", "B. Mallet / Hammer", "C. Wall-nut", "D. Rake" }, 1, "Whack-a-mole style wooden hammer smash!" },

        // 91-100: Advanced Tactics & Trivia
        { "What is the maximum damage dealt by an explosive Cherry Bomb or Jalapeno?", { "A. 500 Damage", "B. 1,000 Damage", "C. 1,800 Damage", "D. 3,000 Damage" }, 2, "1,800 massive damage instantly vanquishes most zombies!" },
        { "Which plant can eat a full Conehead or Buckethead Zombie in one single gulp?", { "A. Chomper", "B. Squash", "C. Potato Mine", "D. Tangle Kelp" }, 0, "One big bite swallows them whole (then chews for 42s)!" },
        { "What musical genre is sung during the credits song by Laura Shigihara?", { "A. Rock and Roll", "B. Pop / Zombies On Your Lawn", "C. Classical", "D. Jazz" }, 1, "'There is a zombie on your lawn...' Pop masterpiece!" },
        { "Which plant costs the most Sun to plant in the entire game?", { "A. Cob Cannon (500 Sun)", "B. Winter Melon (500 Sun total)", "C. Gatling Pea", "D. Twin Sunflower" }, 0, "Cob Cannon costs 500 sun (plus two 100-sun Kernel-pults = 700 total)!" },
        { "Which plant upgrade is the cheapest in Crazy Dave's Twiddydinkies?", { "A. Gatling Pea ($5,000)", "B. Twin Sunflower ($5,000)", "C. Pool Cleaner ($1,000)", "D. Spikerock ($7,500)" }, 2, "Pool Cleaners only cost $1,000 in the shop!" },
        { "What item in Dave's shop automatically kills the very first zombie in a level?", { "A. Garden Rake ($200)", "B. Lawn Mower", "C. Fertilizer", "D. Bug Spray" }, 0, "The Garden Rake steps on the first zombie's head!" },
        { "How many lanes of lawn grass exist in a standard daytime / nighttime level?", { "A. 3 Lanes", "B. 4 Lanes", "C. 5 Lanes", "D. 6 Lanes" }, 2, "5 horizontal lanes (Pool levels expand to 6 lanes)!" },
        { "How many grid columns are there across the lawn from left to right?", { "A. 7 Columns", "B. 8 Columns", "C. 9 Columns", "D. 10 Columns" }, 2, "9 columns across the yard from column 1 to column 9!" },
        { "Which plant is known to generate sleep particles and snoozes during daytime?", { "A. Scaredy-shroom", "B. All nocturnal mushrooms", "C. Chomper", "D. Wall-nut" }, 1, "All mushroom types sleep during daytime without Coffee Bean!" },
        { "What is Crazy Dave's final piece of advice for surviving the zombie apocalypse?", { "A. Eat your vegetables!", "B. Stay CRAAAZY and keep planting!", "C. Run away!", "D. Build a fence!" }, 1, "Stay CRAAAZY and keep defending your lawn!" }
    };
}

void QuizMenu::startQuiz() {
    m_state = QuizState::Playing;
    m_currentQuestionIndex = 0;
    m_score = 0;
    m_coinsEarned = 0;
    m_totalPlayerCoins = ProfileManager::GetInstance().GetActiveProfile().coins;
    m_selectedAnswer = -1;
    m_isCorrect = false;
    m_feedbackTimer = 0.0f;
    m_floatingTexts.clear();

    // Randomly select 10 unique questions from the pool of 100 questions
    m_currentRoundQuestions.clear();
    std::vector<int> indices(m_allQuestions.size());
    for (size_t i = 0; i < indices.size(); ++i) indices[i] = (int)i;
    
    // Fisher-Yates shuffle
    for (int i = (int)indices.size() - 1; i > 0; --i) {
        int j = GetRandomValue(0, i);
        std::swap(indices[i], indices[j]);
    }

    int pickCount = std::min(10, (int)m_allQuestions.size());
    for (int i = 0; i < pickCount; ++i) {
        m_currentRoundQuestions.push_back(m_allQuestions[indices[i]]);
    }

    m_crazyDave.SetAnimation("anim_smalltalk");
    AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/crazydavelong1.ogg"));
}

void QuizMenu::selectAnswer(int optionIndex) {
    if (m_state != QuizState::Playing || m_currentRoundQuestions.empty()) return;

    m_selectedAnswer = optionIndex;
    m_state = QuizState::AnswerFeedback;
    m_feedbackTimer = 0.0f;

    const auto& q = m_currentRoundQuestions[m_currentQuestionIndex];
    if (optionIndex == q.correctIndex) {
        m_isCorrect = true;
        m_score++;
        m_coinsEarned += 300;
        ProfileManager::GetInstance().AddCoins(300);
        m_totalPlayerCoins = ProfileManager::GetInstance().GetActiveProfile().coins;

        m_crazyDave.SetAnimation("anim_crazy");
        AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/crazydaveshort1.ogg"));
        AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/coin.ogg"));
        AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/moneyfalls.ogg"));

        m_floatingTexts.push_back({ "+$300 Cash!", { 200.0f, 220.0f }, 1.0f, GOLD });
    } else {
        m_isCorrect = false;
        m_crazyDave.SetAnimation("anim_mediumtalk");
        AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/crazydavescream.ogg"));
        AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/buzzer.ogg"));

        m_floatingTexts.push_back({ "WRONG!", { 240.0f, 220.0f }, 1.0f, RED });
    }
}

void QuizMenu::nextQuestion() {
    m_currentQuestionIndex++;
    if (m_currentQuestionIndex >= (int)m_currentRoundQuestions.size()) {
        m_state = QuizState::Summary;
        if (m_score >= 7) {
            m_crazyDave.SetAnimation("anim_crazy");
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/crazydaveextralong1.ogg"));
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/winmusic.ogg"));
        } else {
            m_crazyDave.SetAnimation("anim_mediumtalk");
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/crazydavelong2.ogg"));
        }
    } else {
        m_state = QuizState::Playing;
        m_selectedAnswer = -1;
        m_isCorrect = false;
        m_feedbackTimer = 0.0f;
        m_crazyDave.SetAnimation("anim_smalltalk");
        AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/crazydaveshort2.ogg"));
    }
}

void QuizMenu::update(float dt, bool& showQuiz) {
    m_crazyDave.Update(dt);

    // If Dave is playing an expressive animation, return to anim_idle once it finishes 1 cycle
    if (m_crazyDave.GetCurrentAnimName() != "anim_idle") {
        if (m_crazyDave.GetCurrentFrame() >= m_crazyDave.GetEndFrame() - 1) {
            m_crazyDave.SetAnimation("anim_idle");
        }
    }

    // Update floating texts
    for (auto it = m_floatingTexts.begin(); it != m_floatingTexts.end();) {
        it->pos.y -= 30.0f * dt;
        it->alpha -= 0.6f * dt;
        if (it->alpha <= 0.0f) {
            it = m_floatingTexts.erase(it);
        } else {
            ++it;
        }
    }

    Vector2 mousePos = GetVirtualMousePosition();

    // 1. Check Exit / Main Menu button (bottom-right: 615, 548, 164, 26) or ESC key
    if (IsKeyPressed(KEY_ESCAPE)) {
        AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
        showQuiz = false;
        return;
    }

    Rectangle exitRect = { 615.0f, 548.0f, 164.0f, 26.0f };
    if (CheckCollisionPointRec(mousePos, exitRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
        showQuiz = false;
        return;
    }

    // 2. State-specific update
    if (m_state == QuizState::Rules) {
        Rectangle startBtnRect = { 150.0f, 475.0f, 260.0f, 50.0f };
        if (CheckCollisionPointRec(mousePos, startBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
            startQuiz();
        }
    } else if (m_state == QuizState::Playing) {
        // Check 4 answer options (4 stacked buttons)
        for (int i = 0; i < 4; ++i) {
            Rectangle optRect = { 40.0f, 250.0f + (float)i * 54.0f, 480.0f, 46.0f };
            if (CheckCollisionPointRec(mousePos, optRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                selectAnswer(i);
                break;
            }
        }
    } else if (m_state == QuizState::AnswerFeedback) {
        // Only advance when user clicks the NEXT button
        Rectangle nextBtnRect = { 360.0f, 475.0f, 160.0f, 48.0f };
        if (CheckCollisionPointRec(mousePos, nextBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
            nextQuestion();
        }
    } else if (m_state == QuizState::Summary) {
        Rectangle replayBtnRect = { 150.0f, 475.0f, 260.0f, 50.0f };
        if (CheckCollisionPointRec(mousePos, replayBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
            startQuiz();
        }
    }
}

void QuizMenu::drawWrappedText(const BitmapFont& font, const std::string& text, float x, float y, float maxW, float scale, Color color, float lineSpacing) const {
    std::istringstream stream(text);
    std::string word;
    std::string currentLine = "";
    float curY = y;

    while (stream >> word) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        if (font.MeasureText(testLine.c_str(), scale) > (int)maxW && !currentLine.empty()) {
            font.DrawText(currentLine.c_str(), x, curY, scale, color);
            curY += lineSpacing;
            currentLine = word;
        } else {
            currentLine = testLine;
        }
    }
    if (!currentLine.empty()) {
        font.DrawText(currentLine.c_str(), x, curY, scale, color);
    }
}

void QuizMenu::draw() {
    Vector2 mousePos = GetVirtualMousePosition();

    // 1. Draw Challenge Background (800x600)
    if (m_bg.id != 0) {
        DrawTexturePro(m_bg,
                       Rectangle{ 0.0f, 0.0f, (float)m_bg.width, (float)m_bg.height },
                       Rectangle{ 0.0f, 0.0f, 800.0f, 600.0f },
                       Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        ClearBackground(DARKGRAY);
    }

    // 2. Draw Header Title centered (top: y = 25)
    Rectangle titleRect = { 180.0f, 25.0f, 440.0f, 35.0f };
    m_fontTitle.DrawTextCentered("CRAZY DAVE'S BRAIN BUSTERS", { titleRect.x + 2, titleRect.y + 2, titleRect.width, titleRect.height }, 0.55f, BLACK);
    m_fontTitle.DrawTextCentered("CRAZY DAVE'S BRAIN BUSTERS", titleRect, 0.55f, GOLD);

    // 3. Draw Content based on state
    if (m_state == QuizState::Rules) {
        // Rules Card Panel
        Rectangle panelRect = { 40.0f, 95.0f, 480.0f, 440.0f };
        DrawRectangleRec(panelRect, ColorAlpha(BLACK, 0.75f));
        DrawRectangleLinesEx(panelRect, 3.0f, GOLD);

        m_fontTitle.DrawTextCentered("HOW TO PLAY", { panelRect.x, panelRect.y + 18.0f, panelRect.width, 35.0f }, 0.6f, YELLOW);

        float textX = panelRect.x + 25.0f;
        float textY = panelRect.y + 65.0f;
        drawWrappedText(m_fontQuestion, "Greetings, neighbor! Because I'm CRAAAZY, I put together a special Brain Buster trivia quiz for ya!", textX, textY, 430.0f, 1.0f, Color{ 120, 255, 120, 255 }, 26.0f);

        textY += 85.0f;
        drawWrappedText(m_fontQuestion, "* 10 Random questions selected from a bank of 100 PvZ trivia questions.", textX, textY, 430.0f, 0.95f, Color{ 170, 255, 170, 255 }, 24.0f);
        textY += 45.0f;
        drawWrappedText(m_fontQuestion, "* Each correct answer awards you +$300 Gold Cash directly to your bank!", textX, textY, 430.0f, 0.95f, GOLD, 24.0f);
        textY += 45.0f;
        drawWrappedText(m_fontQuestion, "* Answer correctly to prove your zombie-battling IQ!", textX, textY, 430.0f, 0.95f, Color{ 255, 200, 150, 255 }, 24.0f);

        // Start Quiz Button
        Rectangle startBtnRect = { 150.0f, 475.0f, 260.0f, 50.0f };
        bool startHovered = CheckCollisionPointRec(mousePos, startBtnRect);
        Color startCol = startHovered ? Color{ 80, 200, 80, 255 } : Color{ 40, 140, 40, 255 };
        DrawRectangleRec(startBtnRect, startCol);
        DrawRectangleLinesEx(startBtnRect, 3.0f, startHovered ? WHITE : GOLD);
        m_fontOption.DrawTextCentered("START QUIZ", startBtnRect, 0.8f, WHITE);

    } else if (m_state == QuizState::Playing || m_state == QuizState::AnswerFeedback) {
        if (m_currentQuestionIndex >= 0 && m_currentQuestionIndex < (int)m_currentRoundQuestions.size()) {
            const auto& q = m_currentRoundQuestions[m_currentQuestionIndex];

            // Question Card Panel
            Rectangle qCardRect = { 40.0f, 95.0f, 480.0f, 140.0f };
            DrawRectangleRec(qCardRect, ColorAlpha(BLACK, 0.75f));
            DrawRectangleLinesEx(qCardRect, 2.5f, GOLD);

            // Question header: "QUESTION 1 OF 10"
            std::string qNumStr = "QUESTION " + std::to_string(m_currentQuestionIndex + 1) + " OF " + std::to_string(m_currentRoundQuestions.size());
            m_fontStats.DrawText(qNumStr.c_str(), qCardRect.x + 15.0f, qCardRect.y + 12.0f, 0.9f, YELLOW);

            // Question text
            drawWrappedText(m_fontQuestion, q.question, qCardRect.x + 15.0f, qCardRect.y + 38.0f, 450.0f, 1.05f, WHITE, 26.0f);

            // 4 Option Buttons
            for (int i = 0; i < 4; ++i) {
                Rectangle optRect = { 40.0f, 250.0f + (float)i * 54.0f, 480.0f, 46.0f };
                bool hovered = CheckCollisionPointRec(mousePos, optRect);

                Color bgCol = ColorAlpha(BLACK, 0.65f);
                Color borderCol = Color{ 140, 110, 60, 255 };
                Color textCol = WHITE;

                if (m_state == QuizState::Playing) {
                    if (hovered) {
                        bgCol = ColorAlpha(Color{ 80, 70, 30, 255 }, 0.9f);
                        borderCol = YELLOW;
                        textCol = YELLOW;
                    }
                } else if (m_state == QuizState::AnswerFeedback) {
                    if (i == q.correctIndex) {
                        bgCol = ColorAlpha(Color{ 30, 140, 30, 255 }, 0.9f);
                        borderCol = Color{ 100, 255, 100, 255 };
                        textCol = WHITE;
                    } else if (i == m_selectedAnswer && !m_isCorrect) {
                        bgCol = ColorAlpha(Color{ 160, 30, 30, 255 }, 0.9f);
                        borderCol = RED;
                        textCol = Color{ 255, 180, 180, 255 };
                    }
                }

                DrawRectangleRec(optRect, bgCol);
                DrawRectangleLinesEx(optRect, 2.0f, borderCol);
                m_fontOption.DrawTextCentered(q.options[i].c_str(), optRect, 0.7f, textCol);
            }

            // Dave's comment during feedback
            if (m_state == QuizState::AnswerFeedback) {
                Rectangle commentRect = { 40.0f, 475.0f, 300.0f, 60.0f };
                std::string commentText = "Dave: \"" + q.daveComment + "\"";
                drawWrappedText(m_fontQuestion, commentText, commentRect.x, commentRect.y, commentRect.width, 0.9f, Color{ 120, 255, 120, 255 }, 20.0f);

                // Next button
                Rectangle nextBtnRect = { 360.0f, 475.0f, 160.0f, 48.0f };
                bool nextHovered = CheckCollisionPointRec(mousePos, nextBtnRect);
                DrawRectangleRec(nextBtnRect, nextHovered ? Color{ 70, 160, 220, 255 } : Color{ 40, 100, 160, 255 });
                DrawRectangleLinesEx(nextBtnRect, 2.0f, nextHovered ? WHITE : GOLD);
                m_fontOption.DrawTextCentered("NEXT ->", nextBtnRect, 0.75f, WHITE);
            }
        }

    } else if (m_state == QuizState::Summary) {
        // Summary Panel
        Rectangle panelRect = { 40.0f, 95.0f, 480.0f, 440.0f };
        DrawRectangleRec(panelRect, ColorAlpha(BLACK, 0.8f));
        DrawRectangleLinesEx(panelRect, 3.0f, GOLD);

        m_fontTitle.DrawTextCentered("QUIZ COMPLETED!", { panelRect.x, panelRect.y + 18.0f, panelRect.width, 35.0f }, 0.65f, YELLOW);

        // Score display
        std::string scoreStr = "Final Score: " + std::to_string(m_score) + " / " + std::to_string(m_currentRoundQuestions.size());
        m_fontTitle.DrawTextCentered(scoreStr.c_str(), { panelRect.x, panelRect.y + 70.0f, panelRect.width, 35.0f }, 0.55f, WHITE);

        // Cash earned
        std::string earnedStr = "Cash Earned: +" + FormatMoney(m_coinsEarned);
        m_fontTitle.DrawTextCentered(earnedStr.c_str(), { panelRect.x, panelRect.y + 115.0f, panelRect.width, 35.0f }, 0.6f, GOLD);

        // Dave feedback
        float textX = panelRect.x + 30.0f;
        float textY = panelRect.y + 175.0f;
        std::string daveSummary;
        if (m_score == 10) {
            daveSummary = "Dave: \"WABBY WABBO!! A perfect 10/10! You're a certified Backyard Master! The zombies will never pass!\"";
        } else if (m_score >= 7) {
            daveSummary = "Dave: \"Pretty good, neighbor! You definitely have what it takes to defend the garden!\"";
        } else {
            daveSummary = "Dave: \"Argh! Go review your Suburban Almanac and come back for another round!\"";
        }
        drawWrappedText(m_fontQuestion, daveSummary, textX, textY, 420.0f, 1.05f, Color{ 120, 255, 120, 255 }, 26.0f);

        // Replay Button
        Rectangle replayBtnRect = { 150.0f, 475.0f, 260.0f, 50.0f };
        bool replayHovered = CheckCollisionPointRec(mousePos, replayBtnRect);
        Color replayCol = replayHovered ? Color{ 80, 200, 80, 255 } : Color{ 40, 140, 40, 255 };
        DrawRectangleRec(replayBtnRect, replayCol);
        DrawRectangleLinesEx(replayBtnRect, 3.0f, replayHovered ? WHITE : GOLD);
        m_fontOption.DrawTextCentered("PLAY AGAIN", replayBtnRect, 0.8f, WHITE);
    }

    // 4. Draw Animated Crazy Dave (right side)
    m_crazyDave.Draw(490.0f, 75.0f, 0.85f);

    // 5. Draw Bottom Bar OVER Dave (Coin Bank & Main Menu Button)
    // Coin Bank on bottom-left: (20, 545, size 128x31)
    if (m_coinBank.id != 0) {
        DrawTexture(m_coinBank, 20, 545, WHITE);
    } else {
        DrawRectangle(20, 545, 128, 31, ColorAlpha(BLACK, 0.7f));
    }
    std::string moneyStr = FormatMoney(m_totalPlayerCoins);
    Rectangle bankTextRect = { 48.0f, 545.0f, 95.0f, 31.0f };
    m_fontStats.DrawTextCentered(moneyStr.c_str(), bankTextRect, 0.95f, WHITE);

    // Main Menu / Exit Button on bottom-right: (615, 548, 164, 26)
    Rectangle exitRect = { 615.0f, 548.0f, 164.0f, 26.0f };
    bool exitHovered = CheckCollisionPointRec(mousePos, exitRect);
    if (m_closeBtn.id != 0) {
        Texture2D tex = exitHovered ? (m_closeBtnHl.id != 0 ? m_closeBtnHl : m_closeBtn) : m_closeBtn;
        DrawTexturePro(
            tex,
            Rectangle{ 0.0f, 0.0f, (float)tex.width, (float)tex.height },
            exitRect,
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        DrawRectangleRec(exitRect, exitHovered ? DARKGREEN : DARKGRAY);
        DrawRectangleLinesEx(exitRect, 2.0f, GOLD);
    }

    m_fontOption.DrawTextCentered("MAIN MENU", exitRect, 0.70f, exitHovered ? GREEN : Color{ 230, 210, 160, 255 });

    // 6. Draw Floating Texts (e.g. "+$250 Cash!")
    for (const auto& ft : m_floatingTexts) {
        Color c = ColorAlpha(ft.color, ft.alpha);
        m_fontTitle.DrawText(ft.text.c_str(), ft.pos.x, ft.pos.y, 0.6f, c);
    }
}
