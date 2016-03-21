<?php
namespace Home\Controller;
use Think\Controller;
    class PageController extends Controller {

        private function handle($type,$file) {
            $js = file_get_contents($file);

            if(empty($js)) return;
            if(preg_match('/^\xEF\xBB\xBF/',$js))
            {
                $js=substr($js,3);
            }
            //print_r($js);
            switch($type) {
                case 1:
                $db_record = json_decode($js,true);
                //var_dump($db_record);
                M('job_detail')->data($db_record)->add();
                echo "insert one record";
                break;
                case 2:
                break;
                default:
                break;
            }

        }
        public function update_db() {
            //调用此方法，用于更新本地json数据到数据库中
            //更新职位信息
            $job_cache_dir = "./cache/job/";
            
            // Open a known directory, and proceed to read its contents
            if (is_dir($job_cache_dir)) {
                if ($dh = opendir($job_cache_dir)) {
                    while (($file = readdir($dh)) !== false) {
                       if($file != "." && $file != ".."){
                            $this->handle(1,$job_cache_dir.$file);
                        }
                    } 
                    closedir($dh);
                }else {
                    echo "open dir faield";
                }
            }else {
                echo "not a dir ". $job_cache_dir;
            }
            echo "OK";
        }

        public function add() {

            
            $data=array(
               
                "company_des"=>"百度是一家纳斯达克上市公司，是全球最大的中文搜索引擎服务提供商",
                "title"=>"Java开发高级工程师",
                "type"=>"1",
                "logo"=>"./Uploads/Picture/2015-01-01/baidu.jpg",
                "url"=>"http://www.baidu.com",
                "src_desc"=>"百度招聘官网",
                'department'=>'技术部',
                'company'=>'百度科技有限公司',
                'salary'=>'年薪：20-30万',
                'location'=>'北京-西二旗',
                "edu"=>"本科或以上",
                'field'=>'互联网',
                "work_experience"=>"1年以上",
                'pub_time'=>'2014-12-28 19:00:00',
                "head_count"=>"40",
                'label'=>'发展空间大|五险一金|扁平管理',
                'keywords'=>'百度招聘,java开发工程师,互联网',
                'job_require'=>
                    "1、本科以上学历，3年以上java开发经验，具有面向对象方面的知识，能力优秀者可以适当放宽要求|2、精通MySQL和NoSQL数据库操作，熟悉SQL语句调优；|
                    3、具备面向大型网站业务系统的架构设计或开发能力；|4、精通Javascript及AJax，会使用至少一种JS框架进行前端开发；|5、对技术开发的主流技术工具有深入的了解，有较强的新技术学习能力。"
                    ,
                'job_duty'=>
                    "1、从事网络游戏服务器端软件、数据库的开发工作；|2、负责部分程序构架设计及功能模块的开发、调试和维护； |3、根据产品需求进行开发工作量的评估和分析；|4、和团队成员合作，协调完成安排的开发工作"
                    ,
                'company_desc'=>
                    "百度是个什么样的公司？"
                  ,
                'job_welfare'=>
                    '早上9点---下午5点30分。做五休二。偶尔有加班（有餐补）。转正交社保。'
                    
            );


           M('job_detail')->data($data)->add();
           
            
            $data = array(
                    'title'=>"Java学习资料1",
                    'img'=>"./Uploads/Picture/2015-01-01/java1.png",
                    'url'=>"#",
                    'desc'=>"Java学习资料1，教程畅销书推荐",
                    'website'=>'铛铛',
                    'prize'=>'￥ 28.15',
                    'orig_prize'=>'￥ 35.00'
                    );
             M('book_detail')->data($data)->add();
            $data = array(
                    'title'=>"Java学习资料2",
                    'img'=>"./Uploads/Picture/2015-01-01/java2.png",
                    'url'=>"#",
                    'desc'=>"Java学习资料2，附带光盘（2）",
                    'website'=>'铛铛',
                    'prize'=>'￥ 28.15',
                    'orig_prize'=>'￥ 35.00'
                    );

            M('book_detail')->data($data)->add();
            
            $data = array(
                
                    'title'=>"javascript和Ajax学习档案",
                    'desc'=>"教你10分钟学会如何使用javascript和Ajax",
                    'url'=>"#",
                    'keyword'=>'java',
                    'website'=>'CSDN'

                    );
             M('link_detail')->data($data)->add();
                array(
                    'title'=>"百度面试常见问题集锦",
                    'desc'=>"百度近5年的技术面试题及答案，附带前端开发工程师的常见问题分析方法，javascript常见面试题型等",
                    'url'=>"#",
                     'keyword'=>'java',
                    'website'=>'CSDN'
            );  
            M('link_detail')->data($data)->add();
            $data = array(
                
                    'title'=>"加利福尼亚大学：操作系统和系统编程",
                    'play_url' =>'http://v.ifeng.com/include/exterior.swf?guid=510eec2e-24cf-4008-bf23-eb67c32be70a&AutoPlay=false',
                    'img'=>"./Uploads/Picture/2015-01-01/Office-Logo-2013.png",
                    'desc'=>"javascript学习课件.",
                    'website'=>'凤凰卫视',
                    'url'=>"#",
                     'keyword'=>'操作系统'
                    );
            M('course_detail')->data($data)->add();

                array(
                    'title'=>"加利福尼亚大学：操作系统和系统编程",
                    'play_url' =>'http://v.ifeng.com/include/exterior.swf?guid=510eec2e-24cf-4008-bf23-eb67c32be70a&AutoPlay=false',
                    'img'=>"./Uploads/Picture/2015-01-01/Office-Logo-2013.png",
                    'desc'=>"javascript学习课件.",
                    'website'=>'凤凰卫视',
                    'url'=>"#",
                    'keyword'=>'操作系统'
                    
            );  
            M('course_detail')->data($data)->add();

            $data = array(
                
                    'uid' => '1',
                    'user_name' => '翘课老师',
                    'avatar' => './Uploads/Avatar/default.jpg',
                    'user_profile_url' => '#',
                    'comment' => '绝对的好评，不错的网站！',
                    'pub_time' => '2015年1月2日 13:00'
                   
            );
            M('user_comments')->data($data)->add();

            for($i = 0;$i < 3;$i++) {
                $data = array(
                
                    'job_id' => '1',
                    'type' => $i + 1,
                    'res_ids' => '1,2'              
                );
                M('recom_resource')->data($data)->add();
            }

            echo "OK";
        }
        public function index(){
               
        	//职位页面详情页  
            $job_info = $this->get_job_detail(I('id'));
            $recom_books = $this->get_recom_books(I('id'));
            $recom_links = $this->get_recom_links(I('id'));
            $recom_courses = $this->get_recom_courses(I('id'));
            $comments = $this->get_user_comments(I('id'));
            
        	$this->assign('job_info',$job_info);
            $this->assign('recom_books',$recom_books);
            $this->assign('recom_links',$recom_links);
            $this->assign('recom_courses',$recom_courses);
            $this->assign('comments',$comments);

            $this->assign('title','翘课网，让找工作变得简单-职位详情：'.$job_info['title']);
            $this->assign('keywords','翘课网,找工作,在线教育,招聘,求职,'.$job_info['keywords']);
            $this->assign('description','翘课网，让找工作变得简单');

            $this->display();
        }
        private function get_job_detail($id) {

           $dd = M('job_detail')->where(array('id'=>$id))->select();
           $job_info = $dd[0];
           $job_info['label'] = explode('|', $job_info['label']);
           $job_info['job_require'] = explode('|', $job_info['job_require']);
             $job_info['job_duty'] = explode('|', $job_info['job_duty']);
             //$job_info['company_desc'] = explode('|', $job_info['company_desc']);
              $job_info['job_welfare'] = explode('|', $job_info['job_welfare']);
              if($job_info['type'] == "0") {
                $job_info['type'] ="实习生招聘";
              }else {
                $job_info['type'] = "社会招聘";
              }
           return $job_info;
        }
        //type: 1->book,2->link,3->course

        private function get_recom_books($id) {
            $ids_records = M('recom_resource')->field('res_ids')->where('job_id ='.$id.' and type = 1')->select();
            
            if(!empty($ids_records)) {
                $db = M("book_detail"); // 实例化对象
                $recom_books = $db->where('id in ('.$ids_records[0]['res_ids'].')')->select(); 
                return $recom_books;
            }
            //$recom_books = M('book_detail')->limit(3)->select();
            //return $recom_books;
        }
        private function get_recom_links($id) {

            $ids_records = M('recom_resource')->field('res_ids')->where('job_id ='.$id.' and type = 2')->select();
            
            if(!empty($ids_records)) {
                $db = M("link_detail"); // 实例化对象
                $recom_links = $db->where('id in ('.$ids_records[0]['res_ids'].')')->select(); 
                return $recom_links;
            }

            //$recom_links = M('link_detail')->limit(6)->select();
            //return $recom_links;
        }
        private function get_recom_courses($id) {

            $ids_records = M('recom_resource')->field('res_ids')->where('job_id ='.$id.' and type = 3')->select();
            
            if(!empty($ids_records)) {
                $db = M("course_detail"); // 实例化对象
                $recom_courses = $db->where('id in ('.$ids_records[0]['res_ids'].')')->select(); 
                return $recom_courses;
            }

            //$recom_courses = M('course_detail')->limit(3)->select();
            //return $recom_courses;
        }
        private function get_user_comments($id) {
            $comments = M('user_comments')->select();
            return $comments;
        }
    }