sleep 5; ls         // Esperar 5 segundos e listar os conteúdos da diretoria
sleep 5; ls&        // Esperar 5 segundos e listar os conteúdos da diretoria (mas o comando ls corre no background)
(sleep 40;ls)&      // No background, esperar 40 segundos e listar os conteúdos da diretoria
jobs                // Listar os processos no background, ou seja (sleep 40;ls)& [running]
sleep 160           // Esperar 160 segundos
^Z                  // Suspender essa espera
jobs                // Listar os processos a correr no background, se o (sleep40;ls)& ainda estiver a correr, vai listar esse processo [running], bem como o sleep 160 [suspended]
fg                  // Trazer o último processo a correr no background para o foreground
^Z                  // Suspender de novo esse processo
jobs                // Listar os processos no background de novo
bg %1               // Mostrar o processo no background com ID=1
ps –l               // Vai mostrar todos os processos atuais
kill %1             // Termina o processo com ID=1
nano&               // Corre o programa nano no background
^Z                  // Suspende o programa nano
gedit               // Abre o editor de texto gnome
^Z                  // Suspende o editor de text gnome
jobs                // Listar os processos no background
fg %2               // Trazer para o foreground o processo com ID=2
^C                  // Termina o processo acabado de trazer para o foreground
fg                  // Traz para o foreground o último processo no background

