DROP TABLE IF EXISTS job_detail;
CREATE TABLE `job_detail` (
  `id` int(4) NOT NULL AUTO_INCREMENT,
  `url` text DEFAULT NULL,
  `src_desc` char(100) DEFAULT NULL,
  `type` int(4) DEFAULT NULL,
    `catalog` char(50) DEFAULT NULL,
  `title` text DEFAULT NULL,
  `keywords` char(200) DEFAULT NULL,
  `department` char(100) DEFAULT NULL,
  `job_require` text DEFAULT NULL,
  `job_duty` text DEFAULT NULL,
  `job_welfare` text,
  `label` char(100) DEFAULT NULL,
  `company` char(100) DEFAULT NULL,
  `company_desc` text,
  `logo` char(100) DEFAULT NULL,
  `salary` char(50) DEFAULT NULL,
  `work_experience` char(20) DEFAULT NULL,
  `edu` char(20) DEFAULT NULL,
  `field` char(20) DEFAULT NULL,
  `location` char(20) DEFAULT NULL,
  `head_count` int(4) DEFAULT NULL,
  `pub_time` datetime DEFAULT NULL,
  `update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `job_page_feature` (
  `id` int(4) NOT NULL AUTO_INCREMENT,
  `job_id` int(4) NOT NULL,
  `keywords` text NOT NULL,
  `feature` text NOT NULL,
  `pv` int(4) NOT NULL,
  `click` int(4) NOT NULL,
  `update_time` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

CREATE TABLE `course_page_feature` (
  `id` int(4) NOT NULL AUTO_INCREMENT,
  `course_id` int(4) NOT NULL,
  `keywords` text NOT NULL,
  `feature` text NOT NULL,
  `pv` int(4) NOT NULL,
  `click` int(4) NOT NULL,
  `update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS course_detail;
CREATE TABLE `course_detail` (
  `id` int(4) NOT NULL AUTO_INCREMENT,
  `title` char(200) NOT NULL,
  `desc` text NOT NULL,
   `play_url` char(200) NOT NULL,
  `keywords` text NOT NULL,
  `url` text NOT NULL,
  `website` char(100) NOT NULL,
  `img` char(100) DEFAULT NULL,
  `update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS book_detail;
CREATE TABLE `book_detail` (
  `id` int(4) NOT NULL AUTO_INCREMENT,
  `title` char(200) NOT NULL,
  `desc` text NOT NULL,
  `url` char(200) NOT NULL,
  `website` char(100) NOT NULL,
  `img` char(100) DEFAULT NULL,
   `prize` char(20) NOT NULL,
   `orig_prize` char(20) NOT NULL,
  `update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS link_detail;
CREATE TABLE `link_detail` (
  `id` int(4) NOT NULL AUTO_INCREMENT,
  `title` char(200) NOT NULL,
  `desc` text NOT NULL,
  `keywords` text NOT NULL,
  `url` text NOT NULL,
  `website` char(100) NOT NULL,
  `update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

CREATE TABLE `user_comments` (
  `id` int(4) NOT NULL AUTO_INCREMENT,
  `page_id` int(4) NOT NULL,
  `type` int(4) NOT NULL,
  `uid` int(4) NOT NULL,
   `avatar` char(200) NOT NULL,
  `comment` text NOT NULL,
  `user_profile_url` char(200) NOT NULL,
  `pub_time` char(100) NOT NULL,
  `update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS hot_job;
CREATE TABLE `hot_job` (
  `id` int(4) NOT NULL AUTO_INCREMENT,
  `job_id` int(4) NOT NULL,
  `update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS recom_resource;
CREATE TABLE `recom_resource` (
  `id` int(4) NOT NULL AUTO_INCREMENT,
  `job_id` int(4) NOT NULL,
  `type` int(4) NOT NULL,
  `res_ids` text NOT NULL,
  `update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
