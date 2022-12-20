CREATE TABLE IF NOT EXISTS `ladyluck_restore_info` (
  `guid` bigint(20) unsigned DEFAULT NULL,
  `canLoot` bit(1) DEFAULT NULL,
  `map` int(10) unsigned DEFAULT NULL,
  `x` float DEFAULT NULL,
  `y` float DEFAULT NULL,
  `z` float DEFAULT NULL,
  `o` float DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

