# catharsis

**Category**: Reverse

**Author**: cts

**Description**: hgarrereyn said he would rage quit if I made a windows rev so Have fun! Consider this my catharsis from genshin impact

**Public Files**:

- dist/bingus.exe

**Solution:**

Shitty c++ STL rev with lots of anti debug anti VM shit. Some trickery with memory breakpoints and hooking.

Turns the flag into binary then uses the bits to make a JSON blob. Checks the json blob with hardcoded stuff. Last 16 bits may be underconstrained so there are some extra underscores we give to avoid that

Flag: `pbctf{are_y0u_h4ving_fun_yet?!_im_lmaoing_at_ur_life_rn_KPd18iYszPNEA5rdAhF0G3NNpZXl1YSk}__` (missing the underscores at the end is okay)
