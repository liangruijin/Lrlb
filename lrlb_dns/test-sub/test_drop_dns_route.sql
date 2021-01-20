USE lrlb_dns;

SET @time = UNIX_TIMESTAMP(NOW());

DELETE FROM RouteData WHERE modid = 1 and cmdid = 1 and serverport = 9999;

UPDATE RouteVersion SET version = @time WHERE id = 1;

DELETE FROM RouteChange WHERE modid = 1;
