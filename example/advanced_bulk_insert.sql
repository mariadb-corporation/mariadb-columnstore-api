CREATE TABLE `t2` (
  `id` int(11) DEFAULT NULL,
  `name` varchar(40) DEFAULT NULL,
  `dob` date DEFAULT NULL,
  `added` datetime DEFAULT NULL,
  `salary` decimal(9,2) DEFAULT NULL
) ENGINE=Columnstore;
