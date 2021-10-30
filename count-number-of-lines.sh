awk 'END{print NR}' $(fd "[A-z_-]*(.cpp|.hpp|.frag$|.vert$)" .)
