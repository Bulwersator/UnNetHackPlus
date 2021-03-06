/* File modified during XXI century in process of creating UnNetHackPlus */
/*	Copyright (c) 1989 Janet Walz, Mike Threepoint */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "edog.h"
#ifdef USER_SOUNDS
# ifdef USER_SOUNDS_REGEX
#include <regex.h>
# endif
#endif

#ifdef OVLB

static int FDECL(domonnoise,(struct monst *));
static int NDECL(dochat);

#endif /* OVLB */

#ifdef OVL0

static int FDECL(mon_in_room, (struct monst *,int));

static int
mon_in_room(mon, rmtyp)
struct monst *mon;
int rmtyp;
{
    int rno = levl[mon->mx][mon->my].roomno;

    return rooms[rno - ROOMOFFSET].rtype == rmtyp;
}

void
dosounds()
{
    struct mkroom *sroom;
    int hallu, vx, vy;
    struct monst *mtmp;

    if (!flags.soundok || u.uswallow || Underwater) return;

    if (level.sounds && !rn2(level.sounds->freq)) {
	int idx = rn2(level.sounds->n_sounds);
	char *buf;
	struct lvl_sound_bite snd = level.sounds->sounds[idx];
	buf = string_subst(snd.msg);
	switch (snd.flags) {
	default:
	case LVLSND_HEARD:  You_hear(buf);  break;
	case LVLSND_PLINED: pline(buf);     break;
	case LVLSND_VERBAL: verbalize(buf); break;
	case LVLSND_FELT:   You_feel(buf);  break;
	}
    }

    hallu = Hallucination ? 1 : 0;

    if (level.flags.nfountains && !rn2(400)) {
	static const char * const fountain_msg[4] = {
		"bubbling water.",
		"water falling on coins.",
		"the splashing of a naiad.",
		"a soda fountain!",
	};
	You_hear(fountain_msg[rn2(3)+hallu]);
    }
#ifdef SINK
    if (level.flags.nsinks && !rn2(300)) {
	static const char * const sink_msg[3] = {
		"a slow drip.",
		"a gurgling noise.",
		"dishes being washed!",
	};
	You_hear(sink_msg[rn2(2)+hallu]);
    }
#endif
    if (level.flags.has_court && !rn2(200)) {
	static const char * const throne_msg[4] = {
		"the tones of courtly conversation.",
		"a sceptre pounded in judgment.",
		"Someone shouts \"Off with %s head!\"",
		"Queen Beruthiel's cats!",
	};
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->msleeping ||
			is_lord(mtmp->data) || is_prince(mtmp->data)) &&
		!is_animal(mtmp->data) &&
		mon_in_room(mtmp, COURT)) {
		/* finding one is enough, at least for now */
		int which = rn2(3)+hallu;

		if (which != 2) You_hear(throne_msg[which]);
		else		pline(throne_msg[2], uhis());
		return;
	    }
	}
    }
    if (level.flags.has_garden && !rn2(200)) {
	static const char *garden_msg[4] = {
		"crickets chirping!",
		"birds singing!",
		"grass growing!",
		"wind in the willows!",
	};
	You_hear(garden_msg[rn2(2) + 2 * hallu]);
	return;
    }
    if (level.flags.has_swamp && !rn2(200)) {
	static const char * const swamp_msg[3] = {
		"hear mosquitoes!",
		"smell marsh gas!",	/* so it's a smell...*/
		"hear Donald Duck!",
	};
	You(swamp_msg[rn2(2)+hallu]);
	return;
    }
    if (level.flags.has_vault && !rn2(200)) {
	if (!(sroom = search_special(VAULT))) {
	    /* strange ... */
	    level.flags.has_vault = 0;
	    return;
	}
	if(gd_sound())
	    switch (rn2(2)+hallu) {
		case 1: {
		    boolean gold_in_vault = FALSE;

		    for (vx = sroom->lx;vx <= sroom->hx; vx++)
			for (vy = sroom->ly; vy <= sroom->hy; vy++)
			    if (g_at(vx, vy))
				gold_in_vault = TRUE;
		    if (vault_occupied(u.urooms) !=
			 (ROOM_INDEX(sroom) + ROOMOFFSET))
		    {
			if (gold_in_vault)
			    You_hear(!hallu ? "someone counting money." :
				"the quarterback calling the play.");
			else
			    You_hear("someone searching.");
			break;
		    }
		    /* fall into... (yes, even for hallucination) */
		}
		case 0:
		    You_hear("the footsteps of a guard on patrol.");
		    break;
		case 2:
		    You_hear("Ebenezer Scrooge!");
		    break;
	    }
	return;
    }
    if (level.flags.has_beehive && !rn2(200)) {
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->data->mlet == S_ANT && is_flyer(mtmp->data)) &&
		mon_in_room(mtmp, BEEHIVE)) {
		switch (rn2(2)+hallu) {
		    case 0:
			You_hear("a low buzzing.");
			break;
		    case 1:
			You_hear("an angry drone.");
			break;
		    case 2:
			You_hear("bees in your %sbonnet!",
			    uarmh ? "" : "(nonexistent) ");
			break;
		}
		return;
	    }
	}
    }
    if (level.flags.has_lemurepit && !rn2(20)) {
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ( mtmp->data == &mons[PM_LEMURE]
	      && mon_in_room(mtmp, LEMUREPIT) ) {
		if (hallu) {
		    switch (rn2(3)) {
		        case 0:
			    You_hear("screams of lust!");
			    break;
		        case 1:
			    You_hear("the crack of your mistress's whip!");
			    break;
		        case 2:
			    You_hear("a weeping willow!");
			    break;
		    }	
		} else {
		    switch (rn2(6)) {
		        case 0:
			    You_hear("the crack of a barbed whip!");
			    break;
		        case 1:
			    You_hear("the screams of tortured souls!");
			    break;
		        case 2:
			    You_hear("a wail of eternal anguish!");
			    break;
		        case 3:
			    You_hear("diabolical laughter!");
			    break;
		        case 4:
			    You_hear("cries of repentance!");
			    break;
		        case 5:
			    You_hear("futile pleas for mercy!");
			    break;
		    }
		}
		return;
	    }
	}
    }
    if (level.flags.has_morgue && !rn2(200)) {
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (is_undead(mtmp->data) &&
		mon_in_room(mtmp, MORGUE)) {
		switch (rn2(2)+hallu) {
		    case 0:
			You("suddenly realize it is unnaturally quiet.");
			break;
		    case 1:
			pline_The("%s on the back of your %s stands up.",
				body_part(HAIR), body_part(NECK));
			break;
		    case 2:
			pline_The("%s on your %s seems to stand up.",
				body_part(HAIR), body_part(HEAD));
			break;
		}
		return;
	    }
	}
    }
    if (level.flags.has_barracks && !rn2(200)) {
	static const char * const barracks_msg[4] = {
		"blades being honed.",
		"loud snoring.",
		"dice being thrown.",
		"General MacArthur!",
	};
	int count = 0;

	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if (is_mercenary(mtmp->data) &&
		mon_in_room(mtmp, BARRACKS) &&
		/* sleeping implies not-yet-disturbed (usually) */
		(mtmp->msleeping || ++count > 5)) {
		You_hear(barracks_msg[rn2(3)+hallu]);
		return;
	    }
	}
    }
    if (level.flags.has_zoo && !rn2(200)) {
	static const char * const zoo_msg[3] = {
		"a sound reminiscent of an elephant stepping on a peanut.",
		"a sound reminiscent of a seal barking.",
		"Doctor Dolittle!",
	};
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    if ((mtmp->msleeping || is_animal(mtmp->data)) &&
		    mon_in_room(mtmp, ZOO)) {
		You_hear(zoo_msg[rn2(2)+hallu]);
		return;
	    }
	}
    }
    if (level.flags.has_shop && !rn2(200)) {
	if (!(sroom = search_special(ANY_SHOP))) {
	    /* strange... */
	    level.flags.has_shop = 0;
	    return;
	}
	if (tended_shop(sroom) &&
		!index(u.ushops, ROOM_INDEX(sroom) + ROOMOFFSET)) {
	    static const char * const shop_msg[3] = {
		    "someone cursing shoplifters.",
		    "the chime of a cash register.",
		    "Neiman and Marcus arguing!",
	    };
	    You_hear(shop_msg[rn2(2)+hallu]);
	}
	return;
    }
    if (Is_oracle_level(&u.uz) && !rn2(400)) {
	/* make sure the Oracle is still here */
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	    if (!DEADMONSTER(mtmp) && mtmp->data == &mons[PM_ORACLE])
		break;
	/* and don't produce silly effects when she's clearly visible */
	if (mtmp && (hallu || !canseemon(mtmp))) {
	    static const char * const ora_msg[5] = {
		    "a strange wind.",		/* Jupiter at Dodona */
		    "convulsive ravings.",	/* Apollo at Delphi */
		    "snoring snakes.",		/* AEsculapius at Epidaurus */
		    "someone say \"No more woodchucks!\"",
		    "a loud ZOT!"		/* both rec.humor.oracle */
	    };
	    You_hear(ora_msg[rn2(3)+hallu*2]);
	}
	return;
    }
    if (!Is_blackmarket(&u.uz) && at_dgn_entrance("One-eyed Sam's Market") && !rn2(200)) {
      static const char *blkmar_msg[3] = {
        "You hear someone complaining about the prices.",
        "Somebody whispers: \"Food rations? Only 900 zorkmids.\"",
        "You feel like searching for more gold.",
      };
      pline(blkmar_msg[rn2(2)+hallu]);
    }
}

#endif /* OVL0 */
#ifdef OVLB

const char *
growl_sound(mtmp)
struct monst *mtmp;
{
	const char *ret;

	switch (mtmp->data->msound) {
	case MS_MEW:
	case MS_HISS:
	    ret = "hiss";
	    break;
	case MS_BARK:
	case MS_GROWL:
	    ret = "growl";
	    break;
	case MS_ROAR:
	    ret = "roar";
	    break;
	case MS_BUZZ:
	    ret = "buzz";
	    break;
	case MS_SQEEK:
	    ret = "squeal";
	    break;
	case MS_SQAWK:
	    ret = "screech";
	    break;
	case MS_NEIGH:
	    ret = "neigh";
	    break;
	case MS_WAIL:
	    ret = "wail";
	    break;
	case MS_SILENT:
		ret = "commotion";
		break;
	default:
		ret = "scream";
	}
	return ret;
}

/* the sounds of a seriously abused pet, including player attacking it */
void
growl(mtmp)
struct monst *mtmp;
{
    const char *growl_verb = 0;

    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->data->msound)
	return;

    /* presumably nearness and soundok checks have already been made */
    if (Hallucination)
	growl_verb = get_bogus_sound();
    else
	growl_verb = growl_sound(mtmp);
    if (growl_verb) {
	pline("%s %s!", Monnam(mtmp), vtense((char *)0, growl_verb));
	if(flags.run) nomul(0, 0);
	wake_nearto(mtmp->mx, mtmp->my, mtmp->data->mlevel * 18);
    }
}

/* the sounds of mistreated pets */
void
yelp(mtmp)
struct monst *mtmp;
{
    const char *yelp_verb = 0;

    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->data->msound)
	return;

    /* presumably nearness and soundok checks have already been made */
    if (Hallucination)
	yelp_verb = get_bogus_sound();
    else switch (mtmp->data->msound) {
	case MS_MEW:
	    yelp_verb = "yowl";
	    break;
	case MS_BARK:
	case MS_GROWL:
	    yelp_verb = "yelp";
	    break;
	case MS_ROAR:
	    yelp_verb = "snarl";
	    break;
	case MS_SQEEK:
	    yelp_verb = "squeal";
	    break;
	case MS_SQAWK:
	    yelp_verb = "screak";
	    break;
	case MS_WAIL:
	    yelp_verb = "wail";
	    break;
    }
    if (yelp_verb) {
	pline("%s %s!", Monnam(mtmp), vtense((char *)0, yelp_verb));
	if(flags.run) nomul(0, 0);
	wake_nearto(mtmp->mx, mtmp->my, mtmp->data->mlevel * 12);
    }
}

/* the sounds of distressed pets */
void
whimper(mtmp)
struct monst *mtmp;
{
    const char *whimper_verb = 0;

    if (mtmp->msleeping || !mtmp->mcanmove || !mtmp->data->msound)
	return;

    /* presumably nearness and soundok checks have already been made */
    if (Hallucination)
	whimper_verb = get_bogus_sound();
    else switch (mtmp->data->msound) {
	case MS_MEW:
	case MS_GROWL:
	    whimper_verb = "whimper";
	    break;
	case MS_BARK:
	    whimper_verb = "whine";
	    break;
	case MS_SQEEK:
	    whimper_verb = "squeal";
	    break;
    }
    if (whimper_verb) {
	pline("%s %s.", Monnam(mtmp), vtense((char *)0, whimper_verb));
	if(flags.run) nomul(0, 0);
	wake_nearto(mtmp->mx, mtmp->my, mtmp->data->mlevel * 6);
    }
}

/* pet makes "I'm hungry" noises */
void
beg(mtmp)
struct monst *mtmp;
{
    if (mtmp->msleeping || !mtmp->mcanmove ||
	    !(carnivorous(mtmp->data) || herbivorous(mtmp->data)))
	return;

    /* presumably nearness and soundok checks have already been made */
    if (!is_silent(mtmp->data) && mtmp->data->msound <= MS_ANIMAL)
	(void) domonnoise(mtmp);
    else if (mtmp->data->msound >= MS_HUMANOID) {
	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);
	verbalize("I'm hungry.");
    }
}

int
produce_sound(const char* pline_msg, const char*  verbl_msg, struct monst *mtmp)
{
	if (pline_msg) {
		pline("%s %s", Monnam(mtmp), pline_msg);
	} else if (verbl_msg) {
		verbalize(verbl_msg);
	}
	return 1;
}

int
produce_random_spoken_response_from_this_list(struct monst *mtmp, char ** const list, const int size)
{
	verbalize(list[rn2(size)]);
	return 1;
}

int
produce_random_spoken_response_from_list(struct monst *mtmp, char ** const tame, const int t_size, char ** const peaceful, const int p_size, char ** const hostile, const int h_size)
{
	if (mtmp->mtame) {
		return produce_random_spoken_response_from_this_list(mtmp, tame, t_size);
	} else if (mtmp->mpeaceful) {
		return produce_random_spoken_response_from_this_list(mtmp, peaceful, p_size);
	} else {
		return produce_random_spoken_response_from_this_list(mtmp, hostile, h_size);
	}
}

int
vampire_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
	char verbuf[BUFSZ];
	/* vampire messages are varied by tameness, peacefulness, and time of night */
	boolean isnight = night();
	boolean kindred = maybe_polyd(u.umonnum == PM_VAMPIRE || u.umonnum == PM_VAMPIRE_LORD, Race_if(PM_VAMPIRE)); /* DEFERRED u.umonnum == PM_VAMPIRE_MAGE, */
	boolean nightchild = (Upolyd && (u.umonnum == PM_WOLF || u.umonnum == PM_WINTER_WOLF || u.umonnum == PM_WINTER_WOLF_CUB));
	const char *racenoun = (flags.female && urace.individual.f) ?
			urace.individual.f : (urace.individual.m) ?
			urace.individual.m : urace.noun;

	if (mtmp->mtame) {
		if (kindred) {
			Sprintf(verbuf, "Good %s to you Master%s",
				isnight ? "evening" : "day",
				isnight ? "!" : ".  Why do we not rest?");
			verbl_msg = verbuf;
		} else {
			Sprintf(verbuf,"%s%s",
				nightchild ? "Child of the night, " : "",
				midnight() ?
					"I can stand this craving no longer!" :
				isnight ?
					"I beg you, help me satisfy this growing craving!" :
					"I find myself growing a little weary.");
			verbl_msg = verbuf;
		}
	} else if (mtmp->mpeaceful) {
		if (kindred && isnight) {
			Sprintf(verbuf, "Good feeding %s!",
				flags.female ? "sister" : "brother");
			verbl_msg = verbuf;
		} else if (nightchild && isnight) {
			Sprintf(verbuf,
				"How nice to hear you, child of the night!");
				verbl_msg = verbuf;
		} else {
			verbl_msg = "I only drink... potions.";
		}
	} else {
		int vampindex;
		static const char * const vampmsg[] = {
		/* These first two (0 and 1) are specially handled below */
		"I vant to suck your %s!",
		"I vill come after %s without regret!",
		/* other famous vampire quotes can follow here if desired */
		};
		if (kindred) {
			verbl_msg = "This is my hunting ground that you dare to prowl!";
		} else if (youmonst.data == &mons[PM_SILVER_DRAGON] || youmonst.data == &mons[PM_BABY_SILVER_DRAGON]) {
			/* Silver dragons are silver in color, not made of silver */
			Sprintf(verbuf, "%s! Your silver sheen does not frighten me!",
				youmonst.data == &mons[PM_SILVER_DRAGON] ?
				"Fool" : "Young Fool");
			verbl_msg = verbuf; 
		} else {
			vampindex = rn2(SIZE(vampmsg));
			if (vampindex == 0) {
				Sprintf(verbuf, vampmsg[vampindex], body_part(BLOOD));
				verbl_msg = verbuf;
			} else if (vampindex == 1) {
				Sprintf(verbuf, vampmsg[vampindex],
					Upolyd ? an(mons[u.umonnum].mname) : an(racenoun));
				verbl_msg = verbuf;
			} else {
				verbl_msg = vampmsg[vampindex];
			}
		}
	}
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

int
bark_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
	if (flags.moonphase == FULL_MOON && night()) {
		pline_msg = "howls.";
	} else if (mtmp->mpeaceful) {
		if (mtmp->mtame &&
		(mtmp->mconf || mtmp->mflee || mtmp->mtrapped ||
		moves > EDOG(mtmp)->hungrytime || mtmp->mtame < 5)) {
			pline_msg = "whines.";
		} else if (mtmp->mtame && EDOG(mtmp)->hungrytime > moves + 1000) {
			pline_msg = "yips.";
		} else {
			if (mtmp->data != &mons[PM_DINGO]) {/* dingos do not actually bark */
				pline_msg = "barks.";
			}
		}
	} else {
		pline_msg = "growls.";
	}
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

int
grunt_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
	struct permonst *ptr = mtmp->data;
	if ((is_orc(ptr) && !mtmp->mpeaceful) && rn2(10)) {
		char * orc_insults_msgs[] = {
			/* from http://www.anim5.com/wow/generator/dwarf/index.php */
			"I can't hear ye! Scream a wee bit louder ye hairy, putrid whiny little slimey bastitch!",
			"Bite ME will ye? Chew on THIS ye syphilitic panty waist cockroach!",
			"Lemme spell out the rules for ye. I win. YOU LOSE! ye potato faced bug chewin' gutless kidneywipe!",
			"Go ahead an' run! I LIKE a movin' target ye worm livered panty waist wiggly maggot!",
			"Ba ba ba-ba ba you're gonna get murdered",
			"Have a face full o' boot ye bloated rat spawned chunk O' bat spit!",
			"Ye can surrender after I kill yer sorry ass ye worm!",
			"Have a meal of me fist ye rabid spider lovin' greasey dingo buttwipe!",
			/* end of this source */
			"WARGHLBARGLEARGLEBARGLE!", // from http://forum.rpg.net/showthread.php?466187-%28Let-s-Play!%29-Sporkhack/page46
		};
		return produce_random_spoken_response_from_this_list(mtmp, orc_insults_msgs, SIZE(orc_insults_msgs));
	} else if (is_gargoyle(ptr) && rn2(10)) {
		if (mtmp->mpeaceful) {
			pline("%s asks if you've seen Elisa Maza.", Monnam(mtmp));
		} else {
			pline("%s demands to know if you've seen David Xanatos.", Monnam(mtmp));
		}
	} else {
		pline_msg = "grunts.";
	}
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

int
humanoid_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
	if (!mtmp->mpeaceful) {
		if (In_endgame(&u.uz) && is_mplayer(mtmp->data)) {
			return hostile_mplayer_talk(mtmp);
		} else if (is_dwarf(mtmp->data)) {
			if (is_elf(youmonst.data)) {
				char *dwarf_to_elf_insults_msgs[] = {
					"You dendrophile!", //by Volfgarix from bay12forums
					"Hey, pointy-eared tree hugger!" //from SporkHack
				};
				return produce_random_spoken_response_from_this_list(mtmp, dwarf_to_elf_insults_msgs, SIZE(dwarf_to_elf_insults_msgs));
			} else {
				verbl_msg = "Your socks are of inadequate craftmanship!"; //by Shook from bay12forums
			}
		} else if (is_elf(mtmp->data)) {
			if (is_dwarf(youmonst.data)) {
				verbl_msg = "You lawn ornament!"; //from SporkHack
			}
		}
		if(is_dwarf(youmonst.data) && !is_dwarf((mtmp->data))) {
			char *adjectives[] = {
				"tunnel-dwelling", //by el_brio from reddit
				/* by Azba from reddit */
				"short",
				"fat", 
				"barbaric",
				"uncultured",
				"alcoholic",
				"stupid",
				"greedy",
				"hairy",
				"smelly",
			};
			char verbuf[BUFSZ];
			Sprintf(verbuf, "I'm gonna eat your beard for lunch you %s rodent!", adjectives[rn2(SIZE(adjectives))]); //by el_brio from reddit
			char *insult_dwarf_msgs[] = {
				"I don't talk with minerals.", //by vadia from bay12forums
				"Your life will be as short as you!", //by holomanga from reddit
				verbuf
			};
			return produce_random_spoken_response_from_this_list(mtmp, insult_dwarf_msgs, SIZE(insult_dwarf_msgs));
		}
		return 0;	/* no sound */
	}
	/* Generic peaceful humanoid behaviour. */
	if (mtmp->mflee) {
		pline_msg = "wants nothing to do with you.";
	} else if (mtmp->mhp < mtmp->mhpmax/4) {
		pline_msg = "moans.";
	} else if (mtmp->mconf || mtmp->mstun) {
		verbl_msg = !rn2(3) ? "Huh?" : rn2(2) ? "What?" : "Eh?";
	} else if (!mtmp->mcansee) {
		verbl_msg = "I can't see!";
	} else if (mtmp->mtrapped) {
		struct trap *t = t_at(mtmp->mx, mtmp->my);
		if (t) t->tseen = 1;
		verbl_msg = "I'm trapped!";
	} else if (mtmp->mhp < mtmp->mhpmax/2) {
		pline_msg = "asks for a potion of healing.";
	} else if (mtmp->mtame && !mtmp->isminion && moves > EDOG(mtmp)->hungrytime) {
		verbl_msg = "I'm hungry.";
	/* Specific monsters' interests */
	} else if (is_elf(mtmp->data)) {
		pline_msg = "curses orcs.";
	} else if (is_dwarf(mtmp->data)) {
		pline_msg = "talks about mining.";
	} else if (likes_magic(mtmp->data)) {
		pline_msg = "talks about spellcraft.";
	} else if (mtmp->data->mlet == S_CENTAUR) {
		pline_msg = "discusses hunting.";
	} else {
		switch (monsndx(mtmp->data)) {
		case PM_HOBBIT:
			pline_msg = (mtmp->mhpmax - mtmp->mhp >= 10) ?
				"complains about unpleasant dungeon conditions."
				: "asks you about the One Ring.";
			break;
		case PM_ARCHEOLOGIST:
			pline_msg = "describes a recent article in \"Spelunker Today\" magazine.";
			break;
		case PM_TOURIST:
			verbl_msg = "Aloha.";
			break;
		default:
			pline_msg = "discusses dungeon exploration.";
			break;
		}
	}
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

int
neigh_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
	if (mtmp->mtame < 5) {
		pline_msg = "neighs.";
	} else if (moves > EDOG(mtmp)->hungrytime) {
		pline_msg = "whinnies.";
	} else {
		pline_msg = "whickers.";
	}
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

int
werecreature_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
	struct permonst *ptr = mtmp->data;
	if (flags.moonphase == FULL_MOON && (night() ^ !rn2(13))) {
		pline("%s throws back %s head and lets out a blood curdling %s!",
			Monnam(mtmp), mhis(mtmp),
			ptr == &mons[PM_HUMAN_WERERAT] ? "shriek" : "howl");
		wake_nearto(mtmp->mx, mtmp->my, 11*11);
	} else {
		pline_msg = "whispers inaudibly.  All you can make out is \"moon\".";
	}
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

int
soldier_chat(struct monst *mtmp)
{
	char * soldier_foe_msg[] = {
		"Resistance is useless!",
		"You're dog meat!",
		"Surrender!",
	};
	char * soldier_pax_msg[] = {
		"What lousy pay we're getting here!",
		"The food's not fit for Orcs!",
		"My feet hurt, I've been on them all day!",
	};
	return produce_random_spoken_response_from_list(mtmp, soldier_pax_msg, SIZE(soldier_pax_msg), soldier_pax_msg, SIZE(soldier_pax_msg), soldier_foe_msg, SIZE(soldier_foe_msg));
}

int
laugh_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
	static const char * const laugh_msg[4] = {
		"giggles.", "chuckles.", "snickers.", "laughs.",
	};
	pline_msg = laugh_msg[rn2(SIZE(laugh_msg))];
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

int
djinni_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
	struct permonst *ptr = mtmp->data;
	char verbuf[BUFSZ];
	if (mtmp->mtame) {
		if (ptr == &mons[PM_PRISONER]) {
			char *honorific;
			if (is_neuter(youmonst.data)) {
				honorific = "creature";
			} else if(flags.female) {
				honorific = "woman";
			} else {
				honorific = "man";
			}
			/* from http://www.reddit.com/r/nethack/comments/1awkre/looking_for_better_chatting_response_from_tame/ */
			Sprintf(verbuf, "I understand you're a %s who knows how to get things.", honorific); 
			verbl_msg = verbuf;
		} else {
			verbl_msg = "Sorry, I'm all out of wishes.";
		}
	} else if (mtmp->mpeaceful) {
		if (ptr == &mons[PM_WATER_DEMON]) {
			pline_msg = "gurgles.";
		} else {
			verbl_msg = "I'm free!";
		}
	} else {
		verbl_msg = "This will teach you not to disturb me!";
	}
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

int
seduce_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
	if (mtmp->data->mlet != S_NYMPH && could_seduce(mtmp, &youmonst, (struct attack *)0) == 1) {
		(void) doseduce(mtmp);
		return 0;
	}
	switch ((poly_gender() != (int) mtmp->female) ? rn2(3) : 0) {
	case 2:
		verbl_msg = "Hello, sailor.";
	case 1:
		pline_msg = "comes on to you.";
	default:
		pline_msg = "cajoles you.";
	}
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

int
arrest_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
	if (mtmp->mpeaceful) {
		verbalize("Just the facts, %s.", flags.female ? "Ma'am" : "Sir");
	} else {
		static const char * const arrest_msg[3] = {
			"Anything you say can be used against you.",
			"You're under arrest!",
			"Stop in the name of the Law!",
		};
		verbl_msg = arrest_msg[rn2(3)];
	}
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

int
nurse_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
	if (uwep && (uwep->oclass == WEAPON_CLASS || is_weptool(uwep))) {
		verbl_msg = "Put that weapon away before you hurt someone!";
	} else if (uarmc || uarm || uarmh || uarms || uarmg || uarmf) {
		verbl_msg = Role_if(PM_HEALER) ?
		  "Doc, I can't help you unless you cooperate." :
		  "Please undress so I can examine you.";
	} else if (uarmu) {
		verbl_msg = "Take off your shirt, please.";
	} else {
		verbl_msg = "Relax, this won't hurt a bit.";
	}
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

int
guard_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
#ifndef GOLDOBJ
	if (u.ugold) {
#else
		if (money_cnt(invent)) {
#endif
		verbl_msg = "Please drop that gold and follow me.";
	} else {
		verbl_msg = "Please follow me.";
	}
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

int
rider_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
		if (mtmp->data == &mons[PM_DEATH] && !rn2(10)) {
			pline_msg = "is busy reading a copy of Sandman #8.";
		} else {
			verbl_msg = (mtmp->data == &mons[PM_DEATH]) ? "WHO DO YOU THINK YOU ARE, WAR?" : "Who do you think you are, War?";
		}
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

/*
template

int
_chat(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
	char verbuf[BUFSZ];

	return produce_sound(pline_msg, verbl_msg, mtmp);
}
*/

boolean
chatting_impossible(struct permonst *ptr)
{
	/* presumably nearness and sleep checks have already been made */
	if (!flags.soundok) {
		return TRUE;
	}
	if (is_silent(ptr)) {
		return TRUE;
	}
	return FALSE;
}

static int
domonnoise(struct monst *mtmp)
{
	const char *pline_msg = 0; // Monnam(mtmp) will be prepended
	const char *verbl_msg = 0; // verbalize()
	struct permonst *ptr = mtmp->data;

	if(chatting_impossible(ptr)){
		return 0;
	}

	/* Make sure its your role's quest quardian; adjust if not */
	if (ptr->msound == MS_GUARDIAN && ptr != &mons[urole.guardnum]) {
		int mndx = monsndx(ptr);
		ptr = &mons[genus(mndx,1)];
	}

	/* be sure to do this before talking; the monster might teleport away, in
	 * which case we want to check its pre-teleport position
	 */
	if (!canspotmon(mtmp)) {
		map_invisible(mtmp->mx, mtmp->my);
	}

	switch (ptr->msound) {
	case MS_ORACLE:
		return doconsult(mtmp);
	case MS_PRIEST:
		priest_talk(mtmp);
		return 1;
	case MS_LEADER:
	case MS_NEMESIS:
	case MS_GUARDIAN:
		quest_chat(mtmp);
		return 1;
	case MS_SELL: /* pitch, pay, total */
		shk_chat(mtmp);
		return 1;
	case MS_VAMPIRE:
		return vampire_chat(mtmp);
	case MS_WERE:
		return werecreature_chat(mtmp);
	case MS_BARK:
		return bark_chat(mtmp);
	case MS_MEW:
		if (mtmp->mtame) {
			if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped || mtmp->mtame < 5) {
				pline_msg = "yowls.";
			} else if (moves > EDOG(mtmp)->hungrytime) {
				pline_msg = "meows.";
			} else if (EDOG(mtmp)->hungrytime > moves + 1000) {
				pline_msg = "purrs.";
			} else {
				pline_msg = "mews.";
			}
			break;
		} /* else FALLTHRU */
	case MS_GROWL:
#ifdef EXOTIC_PETS
		if (mtmp->mtame && 
		(mtmp->data == &mons[PM_MONKEY] || mtmp->data == &mons[PM_APE] || mtmp->data == &mons[PM_CARNIVOROUS_APE])) {
			if (mtmp->mconf || mtmp->mflee || mtmp->mtrapped || moves > EDOG(mtmp)->hungrytime || mtmp->mtame < 5) {
				pline_msg = "shrieks.";
				wake_nearto(mtmp->mx, mtmp->my, 8*8);
			} else if (EDOG(mtmp)->hungrytime > moves + 1000) {
				pline_msg = "chatters.";
			} else {
				pline_msg = "hoots.";
			}
		} else
#endif
		pline_msg = mtmp->mpeaceful ? "snarls." : "growls!";
		break;
	case MS_ROAR:
		pline_msg = mtmp->mpeaceful ? "snarls." : "roars!";
		break;
	case MS_SQEEK:
		pline_msg = "squeaks.";
		break;
	case MS_SQAWK:
		if (ptr == &mons[PM_RAVEN] && !mtmp->mpeaceful) {
			verbl_msg = "Nevermore!";
		} else {
			pline_msg = "squawks.";
		}
		break;
	case MS_HISS:
		if (!mtmp->mpeaceful) {
			pline_msg = "hisses!";
		} else {
			return 0;	/* no sound */
		}
		break;
	case MS_BUZZ:
		pline_msg = mtmp->mpeaceful ? "drones." : "buzzes angrily.";
		break;
	case MS_GRUNT:
		return grunt_chat(mtmp);
	case MS_NEIGH:
		return neigh_chat(mtmp);
	case MS_WAIL:
		pline_msg = "wails mournfully.";
		break;
	case MS_GURGLE:
		pline_msg = "gurgles.";
		break;
	case MS_BURBLE:
		pline_msg = "burbles.";
		break;
	case MS_SHRIEK:
		pline_msg = "shrieks.";
		aggravate();
		break;
	case MS_IMITATE:
		pline_msg = "imitates you.";
		break;
	case MS_BONES:
		pline("%s rattles noisily.", Monnam(mtmp));
		You("freeze for a moment.");
		nomul(-2, "scared by rattling");
		break;
	case MS_LAUGH:
		return laugh_chat(mtmp);
	case MS_MUMBLE:
		pline_msg = "mumbles incomprehensibly.";
		break;
	case MS_DJINNI:
		return djinni_chat(mtmp);
	case MS_GIANT:
		if (!mtmp->mpeaceful) {
			switch (rn2(4)) {
			case 0: 
				pline("%s boasts about %s gem collection.", Monnam(mtmp), mhis(mtmp));
				break;
			case 1:
				pline_msg = "complains about a diet of mutton.";
				break;
			default:
				pline_msg = "shouts \"Fee Fie Foe Foo!\" and guffaws.";
				wake_nearto(mtmp->mx, mtmp->my, 7*7);
			break;
			}
		break;
		}
		/* else FALLTHRU */
	case MS_HUMANOID:
		return humanoid_chat(mtmp);
	case MS_SEDUCE:
		return seduce_chat(mtmp);
#ifdef KOPS
	case MS_ARREST:
		return arrest_chat(mtmp);
#endif
	case MS_BRIBE:
		if (mtmp->mpeaceful && !mtmp->mtame) {
			(void) demon_talk(mtmp);
			break;
		}
		/* fall through */
	case MS_CUSS:
		if (!mtmp->mpeaceful) {
			cuss(mtmp);
		}
		break;
	case MS_SPELL:
		/* deliberately vague, since it's not actually casting any spell */
		pline_msg = "seems to mutter a cantrip.";
		break;
	case MS_NURSE:
		return nurse_chat(mtmp);
	break;
	case MS_GUARD:
		return guard_chat(mtmp);
	case MS_SOLDIER:
		return soldier_chat(mtmp);
	case MS_RIDER:
		return rider_chat(mtmp);
	}
	return produce_sound(pline_msg, verbl_msg, mtmp);
}

int
dotalk()
{
    int result;
    boolean save_soundok = flags.soundok;
    flags.soundok = 1;	/* always allow sounds while chatting */
    result = dochat();
    flags.soundok = save_soundok;
    return result;
}

static int
dochat()
{
    struct monst *mtmp;
    int tx,ty;
    struct obj *otmp;
    int mon_count = 0;
    int dx,dy;

    if (is_silent(youmonst.data)) {
	pline("As %s, you cannot speak.", an(youmonst.data->mname));
	return(0);
    }
    if (Strangled) {
	You_cant("speak.  You're choking!");
	return(0);
    }
    if (u.uswallow) {
	pline("They won't hear you out there.");
	return(0);
    }
    if (Underwater) {
	Your("speech is unintelligible underwater.");
	return(0);
    }

    if (!Blind && (otmp = shop_object(u.ux, u.uy)) != (struct obj *)0) {
	/* standing on something in a shop and chatting causes the shopkeeper
	   to describe the price(s).  This can inhibit other chatting inside
	   a shop, but that shouldn't matter much.  shop_object() returns an
	   object iff inside a shop and the shopkeeper is present and willing
	   (not angry) and able (not asleep) to speak and the position contains
	   any objects other than just gold.
	*/
	price_quote(otmp);
	return(1);
    }

    /* count the monsters surrounding the player */
    u.dx = u.dy = u.dz = 0;
    for (dx = -1; dx <= +1; dx++) {
	for (dy = -1; dy <= +1; dy++) {
	    if (u.ux+dx == u.ux && u.uy+dy == u.uy) continue;
	    mtmp = m_at(u.ux+dx, u.uy+dy);
	    if (mtmp && canspotmon(mtmp)) {
		mon_count++;
		u.dx = dx; u.dy = dy;
	    }
	}
    }

    /* only ask for directions if there is more or less than one monster
     * around */
    if ((mon_count != 1 || iflags.vanilla_ui_behavior) &&
        !getdir("Talk to whom? (in what direction)")) {
	/* decided not to chat */
	return(0);
    }

    if (u.usteed && u.dz > 0) {
	return (domonnoise(u.usteed));
    }
    if (u.dz) {
	pline("They won't hear you %s there.", u.dz < 0 ? "up" : "down");
	return(0);
    }

	if (u.dx == 0 && u.dy == 0) {
		/*
		* It raises all sorts of questions: can you wear
		* 2 helmets, 2 amulets, 3 pairs of gloves or 6 rings as a marilith, etc...
		* But similar things appear in decapitation messages, so there is no point in not using this.
		*/
		if (has_two_heads(youmonst.data)) {
			You("discover that your other head makes boring conversation.");
			return(1);
		}
		pline("Talking to yourself is a bad habit for a dungeoneer.");
		return(0);
	}

    tx = u.ux+u.dx; ty = u.uy+u.dy;
    mtmp = m_at(tx, ty);

    if (!mtmp || mtmp->mundetected ||
		mtmp->m_ap_type == M_AP_FURNITURE ||
		mtmp->m_ap_type == M_AP_OBJECT)
	return(0);

    /* sleeping monsters won't talk, except priests (who wake up) */
    if ((!mtmp->mcanmove || mtmp->msleeping) && !mtmp->ispriest) {
	/* If it is unseen, the player can't tell the difference between
	   not noticing him and just not existing, so skip the message. */
	if (canspotmon(mtmp))
	    pline("%s seems not to notice you.", Monnam(mtmp));
	return(0);
    }

    /* if this monster is waiting for something, prod it into action */
    mtmp->mstrategy &= ~STRAT_WAITMASK;

    if (mtmp->mtame && mtmp->meating) {
	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);
	pline("%s is eating noisily.", Monnam(mtmp));
	return (0);
    }

    return domonnoise(mtmp);
}

#ifdef USER_SOUNDS

extern void FDECL(play_usersound, (const char*, int));

typedef struct audio_mapping_rec {
#ifdef USER_SOUNDS_REGEX
	struct re_pattern_buffer regex;
#else
	char *pattern;
#endif
	char *filename;
	int volume;
	struct audio_mapping_rec *next;
} audio_mapping;

static audio_mapping *soundmap = 0;

char* sounddir = ".";

/* adds a sound file mapping, returns 0 on failure, 1 on success */
int
add_sound_mapping(mapping)
const char *mapping;
{
	char text[256];
	char filename[256];
	char filespec[256];
	int volume;

	if (sscanf(mapping, "MESG \"%255[^\"]\"%*[\t ]\"%255[^\"]\" %d",
		   text, filename, &volume) == 3) {
	    const char *err;
	    audio_mapping *new_map;

	    if (strlen(sounddir) + strlen(filename) > 254) {
		raw_print("sound file name too long");
		return 0;
	    }
	    Sprintf(filespec, "%s/%s", sounddir, filename);

	    if (can_read_file(filespec)) {
		new_map = (audio_mapping *)alloc(sizeof(audio_mapping));
#ifdef USER_SOUNDS_REGEX
		new_map->regex.translate = 0;
		new_map->regex.fastmap = 0;
		new_map->regex.buffer = 0;
		new_map->regex.allocated = 0;
		new_map->regex.regs_allocated = REGS_FIXED;
#else
		new_map->pattern = (char *)alloc(strlen(text) + 1);
		Strcpy(new_map->pattern, text);
#endif
		new_map->filename = strdup(filespec);
		new_map->volume = volume;
		new_map->next = soundmap;

#ifdef USER_SOUNDS_REGEX
		err = re_compile_pattern(text, strlen(text), &new_map->regex);
#else
		err = 0;
#endif
		if (err) {
		    raw_print(err);
		    free(new_map->filename);
		    free(new_map);
		    return 0;
		} else {
		    soundmap = new_map;
		}
	    } else {
		Sprintf(text, "cannot read %.243s", filespec);
		raw_print(text);
		return 0;
	    }
	} else {
	    raw_print("syntax error in SOUND");
	    return 0;
	}

	return 1;
}

void
play_sound_for_message(msg)
const char* msg;
{
	audio_mapping* cursor = soundmap;

	while (cursor) {
#ifdef USER_SOUNDS_REGEX
	    if (re_search(&cursor->regex, msg, strlen(msg), 0, 9999, 0) >= 0) {
#else
	    if (pmatch(cursor->pattern, msg)) {
#endif
		play_usersound(cursor->filename, cursor->volume);
	    }
	    cursor = cursor->next;
	}
}

#endif /* USER_SOUNDS */

#endif /* OVLB */

/*sounds.c*/
