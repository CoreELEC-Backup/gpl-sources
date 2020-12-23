local unit1 = system.loadunit 'unit1'
local forms = system.loadunit 'forms'

local keymap = {
  left  = { forms.vk_left,  'Left' },
  right = { forms.vk_right, 'Right' },
  a     = { forms.vk_right, 'Right' },
  l1    = { 49,             'Game A' },
  r1    = { 50,             'Game B' },
  l2    = { 51,             'Select' }
}

local menu = {
  { unit1.form1.btn_game_a_down, 'Game A', 49 },
  { unit1.form1.btn_game_b_down, 'Game B', 50 },
  { unit1.form1.btn_mode_down,   'Select', 51 }
}

local timers = {
  unit1.form1.timerbonus,
  unit1.form1.timerclock,
  unit1.form1.timerdate,
  unit1.form1.timergame,
  unit1.form1.timergameover,
  unit1.form1.timergamestart,
  unit1.form1.timergot,
  unit1.form1.timerreprise,
  unit1.form1.timerstartup
}

unit1.pfgameover.data = system.loadbin( 'Gameover.pcm' )
unit1.pfgot.data = system.loadbin( 'Got.pcm' )
unit1.pfmiss.data = system.loadbin( 'Miss.pcm' )
unit1.pfmove.data = system.loadbin( 'Move.pcm' )
unit1.pfstart.data = system.loadbin( 'Start.pcm' )

unit1.form1.oncreate()

unit1.bsound = true
unit1.imode = 0
unit1.form1.gam_setmode()

return system.init(
  unit1.form1.im_background,
  keymap,
  function( key ) unit1.form1.onkeydown( nil, key, 0 ) end,
  function( key ) unit1.form1.onkeyup( nil, key, 0 ) end,
  timers,
  { 183, 382, 116, 145 },
  menu
)
