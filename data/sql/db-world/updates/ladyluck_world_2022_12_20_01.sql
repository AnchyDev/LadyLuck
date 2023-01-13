ALTER TABLE `ladyluck_lottery_loot` ADD COLUMN `level_min` INT NULL DEFAULT '0' AFTER `item_count`;
ALTER TABLE `ladyluck_lottery_loot` ADD COLUMN `level_max` INT NULL DEFAULT '80' AFTER `level_min`;