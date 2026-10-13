#include "types.h"
#include <stdlib.h>

const Portao *tipo_para_portao(const TipoElemento *tipo) {
  if (*tipo != PORTAO) return NULL;

  return (const Portao *) tipo;
}